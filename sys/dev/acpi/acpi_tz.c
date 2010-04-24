/* $NetBSD: acpi_tz.c,v 1.68 2010/04/24 06:31:44 jruoho Exp $ */

/*
 * Copyright (c) 2003 Jared D. McNeill <jmcneill@invisible.ca>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * ACPI Thermal Zone driver
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: acpi_tz.c,v 1.68 2010/04/24 06:31:44 jruoho Exp $");

#include <sys/param.h>
#include <sys/device.h>
#include <sys/callout.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <dev/acpi/acpireg.h>
#include <dev/acpi/acpivar.h>
#include <dev/acpi/acpi_power.h>

#define _COMPONENT          ACPI_TZ_COMPONENT
ACPI_MODULE_NAME            ("acpi_tz")

/* flags */
#define ATZ_F_VERBOSE		0x01	/* show events to console */
#define ATZ_F_CRITICAL		0x02	/* zone critical */
#define ATZ_F_HOT		0x04	/* zone hot */
#define ATZ_F_PASSIVE		0x08	/* zone passive cooling */
#define ATZ_F_PASSIVEONLY	0x10	/* zone is passive cooling only */

/* no active cooling level */
#define ATZ_ACTIVE_NONE -1

/* constants */
#define ATZ_TZP_RATE	300	/* default if no _TZP CM present (30 secs) */
#define ATZ_NLEVELS	10	/* number of cooling levels, from ACPI spec */
#define ATZ_ZEROC	2732	/* 0C, measured in 0.1 Kelvin */
#define ATZ_TMP_INVALID	0xffffffff	/* invalid temperature */
#define ATZ_ZONE_EXPIRE	9000	/* zone info refetch interval (15min) */

/* sensor indexes */
#define ATZ_SENSOR_TEMP	0	/* thermal zone temperature */

/*
 * ACPI Temperature Zone information. Note all temperatures are reported
 * in 0.1 Kelvin, and that the ACPI specification assumes that
 * K = C + 273.2 rather than the nominal 273.15 used by envsys(4).
 * So define an appropriate conversion.
 */

#define	ATZ2UKELVIN(t) ((t) * 100000 - 50000)

struct acpitz_zone {
	/* Active cooling temperature threshold */
	uint32_t ac[ATZ_NLEVELS];
	/* Package of references to all active cooling devices for a level */
	ACPI_BUFFER al[ATZ_NLEVELS];
	/* Critical temperature threshold for system shutdown */
	uint32_t crt;
	/* Critical temperature threshold for S4 sleep */
	uint32_t hot;
	/* Package of references to processor objects for passive cooling */
	ACPI_BUFFER psl;
	/* Conveys if temperatures are absolute or relative values. */
	uint32_t rtv;
	/* Passive cooling temperature threshold */
	uint32_t psv;
	/* Thermal constants for use in passive cooling formulas */
	uint32_t tc1, tc2;
	/* Current temperature of the thermal zone */
	uint32_t prevtmp, tmp;
	/* Thermal sampling period for passive cooling, in tenths of seconds */
	uint32_t tsp;
	/* Package of references to devices in this TZ (optional) */
	ACPI_BUFFER tzd;
	/* Recommended TZ polling frequency, in tenths of seconds */
	uint32_t tzp;
	/* Thermal zone name */
	char *name;
	/* FAN min, max, current rpms */
	uint32_t fanmin, fanmax, fancurrent;
};

struct acpitz_softc {
	struct acpi_devnode *sc_devnode;
	struct acpitz_zone sc_zone;
	struct callout sc_callout;
	struct sysmon_envsys *sc_sme;
	envsys_data_t sc_temp_sensor;
	envsys_data_t sc_fan_sensor;
	int sc_active;		/* active cooling level */
	int sc_flags;
	int sc_rate;		/* tz poll rate */
	int sc_zone_expire;

	int sc_first;
	int sc_have_fan;	/* FAN sensor is optional */
};

static int	acpitz_match(device_t, cfdata_t, void *);
static void	acpitz_attach(device_t, device_t, void *);
static int	acpitz_detach(device_t, int);

static void	acpitz_get_status(void *);
static void	acpitz_get_zone(void *, int);
static void	acpitz_get_zone_quiet(void *);
static char	*acpitz_celcius_string(int);
static void	acpitz_print_status(device_t);
static void	acpitz_power_off(struct acpitz_softc *);
static void	acpitz_power_zone(struct acpitz_softc *, int, int);
static void	acpitz_sane_temp(uint32_t *tmp);
static ACPI_STATUS
		acpitz_switch_cooler(ACPI_OBJECT *, void *);
static void	acpitz_notify_handler(ACPI_HANDLE, uint32_t, void *);
static int	acpitz_get_integer(device_t, const char *, uint32_t *);
static void	acpitz_tick(void *);
static void	acpitz_init_envsys(device_t);
static void	acpitz_get_limits(struct sysmon_envsys *, envsys_data_t *,
				  sysmon_envsys_lim_t *, uint32_t *);
static int	acpitz_get_fanspeed(device_t, uint32_t *,
				    uint32_t *, uint32_t *);
#ifdef notyet
static ACPI_STATUS
		acpitz_set_fanspeed(device_t, uint32_t);
#endif

CFATTACH_DECL_NEW(acpitz, sizeof(struct acpitz_softc),
    acpitz_match, acpitz_attach, acpitz_detach, NULL);

/*
 * acpitz_match: autoconf(9) match routine
 */
static int
acpitz_match(device_t parent, cfdata_t match, void *aux)
{
	struct acpi_attach_args *aa = aux;

	if (aa->aa_node->ad_type != ACPI_TYPE_THERMAL)
		return 0;

	return 1;
}

/*
 * acpitz_attach: autoconf(9) attach routine
 */
static void
acpitz_attach(device_t parent, device_t self, void *aux)
{
	struct acpitz_softc *sc = device_private(self);
	struct acpi_attach_args *aa = aux;
	ACPI_STATUS rv;
	ACPI_INTEGER v;

#if 0
	sc->sc_flags = ATZ_F_VERBOSE;
#endif
	sc->sc_devnode = aa->aa_node;

	aprint_naive("\n");
	aprint_normal(": ACPI Thermal Zone\n");

	rv = acpi_eval_integer(sc->sc_devnode->ad_handle, "_TZP", &v);
	if (ACPI_FAILURE(rv))
		sc->sc_zone.tzp = ATZ_TZP_RATE;
	else
		sc->sc_zone.tzp = v;

	aprint_debug_dev(self, "sample rate %d.%ds\n",
	    sc->sc_zone.tzp / 10, sc->sc_zone.tzp % 10);

	/* XXX a value of 0 means "polling is not necessary" */
	if (sc->sc_zone.tzp == 0)
		sc->sc_zone.tzp = ATZ_TZP_RATE;

	sc->sc_zone_expire = ATZ_ZONE_EXPIRE / sc->sc_zone.tzp;
	sc->sc_first = 1;
	sc->sc_have_fan = 0;
	if (acpitz_get_fanspeed(self,
	    &sc->sc_zone.fanmin, &sc->sc_zone.fanmax, &sc->sc_zone.fancurrent)
	    == 0)
		sc->sc_have_fan = 1;

	rv = acpi_eval_string(sc->sc_devnode->ad_handle,
	    "REGN", &sc->sc_zone.name);

	if (ACPI_FAILURE(rv))
		sc->sc_zone.name = __UNCONST("temperature");

	acpitz_get_zone(self, 1);
	acpitz_get_status(self);

	(void)pmf_device_register(self, NULL, NULL);
	(void)acpi_register_notify(sc->sc_devnode, acpitz_notify_handler);

	callout_init(&sc->sc_callout, CALLOUT_MPSAFE);
	callout_setfunc(&sc->sc_callout, acpitz_tick, self);

	acpitz_init_envsys(self);

	callout_schedule(&sc->sc_callout, sc->sc_zone.tzp * hz / 10);
}

static int
acpitz_detach(device_t self, int flags)
{
	struct acpitz_softc *sc = device_private(self);
	ACPI_HANDLE hdl;
	ACPI_BUFFER al;
	ACPI_STATUS rv;
	int i;

	callout_halt(&sc->sc_callout, NULL);
	callout_destroy(&sc->sc_callout);

	pmf_device_deregister(self);
	acpi_deregister_notify(sc->sc_devnode);

	/*
	 * Although the device itself should not contain any power
	 * resources, we have possibly used the resources of active
	 * cooling devices. To unregister these, first fetch a fresh
	 * active cooling zone, and then detach the resources from
	 * the reference handles contained in the cooling zone.
	 */
	acpitz_get_zone(self, 0);

	for (i = 0; i < ATZ_NLEVELS; i++) {

		if (sc->sc_zone.al[i].Pointer == NULL)
			continue;

		al = sc->sc_zone.al[i];
		rv = acpi_eval_reference_handle(al.Pointer, &hdl);

		if (ACPI_SUCCESS(rv))
			acpi_power_deregister_from_handle(hdl);

		ACPI_FREE(sc->sc_zone.al[i].Pointer);
	}

	if (sc->sc_sme != NULL)
		sysmon_envsys_unregister(sc->sc_sme);

	return 0;
}

static void
acpitz_get_zone_quiet(void *opaque)
{
	acpitz_get_zone(opaque, 0);
}

static void
acpitz_get_status(void *opaque)
{
	device_t dv = opaque;
	struct acpitz_softc *sc = device_private(dv);
	uint32_t tmp, active, fmin, fmax, fcurrent;
	int i, flags;

	sc->sc_zone_expire--;
	if (sc->sc_zone_expire <= 0) {
		sc->sc_zone_expire = ATZ_ZONE_EXPIRE / sc->sc_zone.tzp;
		if (sc->sc_flags & ATZ_F_VERBOSE)
			printf("%s: force refetch zone\n", device_xname(dv));
		acpitz_get_zone(dv, 0);
	}

	if (acpitz_get_integer(dv, "_TMP", &tmp) != 0)
		return;

	sc->sc_zone.prevtmp = sc->sc_zone.tmp;
	sc->sc_zone.tmp = tmp;
	if (sc->sc_first)
		sc->sc_zone.prevtmp = tmp;
	/* XXX sanity check for tmp here? */

	if (acpitz_get_fanspeed(dv, &fmin, &fmax, &fcurrent) == 0) {
		if (fcurrent != ATZ_TMP_INVALID)
			sc->sc_zone.fancurrent = fcurrent;
	}

	/*
	 * The temperature unit for envsys(4) is microKelvin, so convert to
	 * that from ACPI's microKelvin. Also, the ACPI specification assumes
	 * that K = C + 273.2 rather than the nominal 273.15 used by envsys(4),
	 * so we correct for that too.
	 */
	sc->sc_temp_sensor.value_cur = ATZ2UKELVIN(sc->sc_zone.tmp);
	sc->sc_temp_sensor.state = ENVSYS_SVALID;

	sc->sc_fan_sensor.value_cur = sc->sc_zone.fancurrent;
	sc->sc_fan_sensor.state = ENVSYS_SVALID;

	if (sc->sc_flags & ATZ_F_VERBOSE)
		acpitz_print_status(dv);

	if (sc->sc_flags & ATZ_F_PASSIVEONLY) {
		/* Passive Cooling: XXX not yet */

	} else {
		/* Active Cooling */

		/* temperature threshold: _AC0 > ... > _AC9 */
		active = ATZ_ACTIVE_NONE;
		for (i = ATZ_NLEVELS - 1; i >= 0; i--) {
			if (sc->sc_zone.ac[i] == ATZ_TMP_INVALID)
				continue;

			/* we want to keep highest cooling mode in 'active' */
			if (sc->sc_zone.ac[i] <= tmp)
				active = i;
		}

		flags = sc->sc_flags &
		    ~(ATZ_F_CRITICAL|ATZ_F_HOT|ATZ_F_PASSIVE);
		if (sc->sc_zone.psv != ATZ_TMP_INVALID &&
		    tmp >= sc->sc_zone.psv)
			flags |= ATZ_F_PASSIVE;
		if (sc->sc_zone.hot != ATZ_TMP_INVALID &&
		    tmp >= sc->sc_zone.hot)
			flags |= ATZ_F_HOT;
		if (sc->sc_zone.crt != ATZ_TMP_INVALID &&
		    tmp >= sc->sc_zone.crt)
			flags |= ATZ_F_CRITICAL;

		if (flags != sc->sc_flags) {
			int changed = (sc->sc_flags ^ flags) & flags;
			sc->sc_flags = flags;
			if (changed & ATZ_F_CRITICAL) {
				sc->sc_temp_sensor.state = ENVSYS_SCRITOVER;
				aprint_debug_dev(dv,
				    "zone went critical at temp %sC\n",
				    acpitz_celcius_string(tmp));
			} else if (changed & ATZ_F_HOT) {
				sc->sc_temp_sensor.state = ENVSYS_SCRITOVER;
				aprint_debug_dev(dv,
				    "zone went hot at temp %sC\n",
				    acpitz_celcius_string(tmp));
			}
		}

		/* power on fans */
		if (sc->sc_active != active) {
			if (sc->sc_active != ATZ_ACTIVE_NONE)
				acpitz_power_zone(sc, sc->sc_active, 0);

			if (active != ATZ_ACTIVE_NONE) {
				if (sc->sc_flags & ATZ_F_VERBOSE)
					printf("%s: active cooling level %u\n",
					    device_xname(dv), active);
				acpitz_power_zone(sc, active, 1);
			} else if (sc->sc_flags & ATZ_F_VERBOSE)
				printf("%s: no active cooling level\n",
				    device_xname(dv));

			sc->sc_active = active;
		}
	}

	return;
}

static char *
acpitz_celcius_string(int dk)
{
	static char buf[10];
	int dc;

	dc = abs(dk - ATZ_ZEROC);
	snprintf(buf, sizeof(buf), "%s%d.%d", (dk >= ATZ_ZEROC)?"":"-",
		dc / 10, dc % 10);

	return buf;
}

static void
acpitz_print_status(device_t dv)
{
	struct acpitz_softc *sc = device_private(dv);

	printf("%s: zone temperature is now %sC\n", device_xname(dv),
	    acpitz_celcius_string(sc->sc_zone.tmp));
	if (sc->sc_have_fan) {
		printf("%s: fan rpm %u\n", device_xname(dv),
		    sc->sc_zone.fancurrent);
	}

	return;
}

static ACPI_STATUS
acpitz_switch_cooler(ACPI_OBJECT *obj, void *arg)
{
	int flag, pwr_state;
	ACPI_HANDLE cooler;
	ACPI_STATUS rv;

	/*
	 * The _ALx object is a package in which the elements
	 * are reference handles to an active cooling device
	 * (typically PNP0C0B, ACPI fan device). Try to turn
	 * on (or off) the power resources behind these handles
	 * to start (or terminate) the active cooling.
	 */
	flag = *(int *)arg;
	pwr_state = (flag != 0) ? ACPI_STATE_D0 : ACPI_STATE_D3;

	rv = acpi_eval_reference_handle(obj, &cooler);

	if (ACPI_FAILURE(rv)) {
		aprint_error("%s: failed to get reference handle\n", __func__);
		return rv;
	}

	(void)acpi_power_set_from_handle(cooler, pwr_state);

	return AE_OK;
}

/*
 * acpitz_power_zone:
 *	power on or off the i:th part of the zone zone
 */
static void
acpitz_power_zone(struct acpitz_softc *sc, int i, int on)
{
	KASSERT(i >= 0 && i < ATZ_NLEVELS);

	acpi_foreach_package_object(sc->sc_zone.al[i].Pointer,
	    acpitz_switch_cooler, &on);
}


/*
 * acpitz_power_off:
 *	power off parts of the zone
 */
static void
acpitz_power_off(struct acpitz_softc *sc)
{
	int i;

	for (i = 0 ; i < ATZ_NLEVELS; i++) {
		if (sc->sc_zone.al[i].Pointer == NULL)
			continue;
		acpitz_power_zone(sc, i, 0);
	}
	sc->sc_active = ATZ_ACTIVE_NONE;
	sc->sc_flags &= ~(ATZ_F_CRITICAL|ATZ_F_HOT|ATZ_F_PASSIVE);
}

static void
acpitz_get_zone(void *opaque, int verbose)
{
	device_t dv = opaque;
	struct acpitz_softc *sc = device_private(dv);
	ACPI_STATUS rv;
	char buf[8];
	int i, valid_levels;

	if (!sc->sc_first) {
		acpitz_power_off(sc);

		for (i = 0; i < ATZ_NLEVELS; i++) {
			if (sc->sc_zone.al[i].Pointer != NULL)
				ACPI_FREE(sc->sc_zone.al[i].Pointer);
			sc->sc_zone.al[i].Pointer = NULL;
		}
	}

	valid_levels = 0;

	for (i = 0; i < ATZ_NLEVELS; i++) {
		ACPI_OBJECT *obj;

		snprintf(buf, sizeof(buf), "_AC%d", i);
		if (acpitz_get_integer(dv, buf, &sc->sc_zone.ac[i]))
			continue;

		snprintf(buf, sizeof(buf), "_AL%d", i);
		rv = acpi_eval_struct(sc->sc_devnode->ad_handle, buf,
		    &sc->sc_zone.al[i]);
		if (ACPI_FAILURE(rv)) {
			sc->sc_zone.al[i].Pointer = NULL;
			continue;
		}

		obj = sc->sc_zone.al[i].Pointer;
		if (obj != NULL) {
			if (obj->Type != ACPI_TYPE_PACKAGE) {
				aprint_error("%d not package\n", i);
				ACPI_FREE(obj);
				sc->sc_zone.al[i].Pointer = NULL;
				continue;
			}
		}

		if (sc->sc_first)
			aprint_normal(" active cooling level %d: %sC", i,
			    acpitz_celcius_string(sc->sc_zone.ac[i]));

		valid_levels++;
	}

	acpitz_get_integer(dv, "_TMP", &sc->sc_zone.tmp);
	acpitz_get_integer(dv, "_CRT", &sc->sc_zone.crt);
	acpitz_get_integer(dv, "_HOT", &sc->sc_zone.hot);
	acpitz_get_integer(dv, "_PSV", &sc->sc_zone.psv);
	acpitz_get_integer(dv, "_TC1", &sc->sc_zone.tc1);
	acpitz_get_integer(dv, "_TC2", &sc->sc_zone.tc2);

#if 0
	sc->sc_zone.psl.Length = ACPI_ALLOCATE_LOCAL_BUFFER;
	sc->sc_zone.psl.Pointer = NULL;
	AcpiEvaluateObject(sc->sc_devnode->ad_handle,
	    "_PSL", NULL, &sc->sc_zone.psl);
#endif

	/* ACPI spec: If _RTV is not present or present and zero,
	 * values are absolute. */
	acpitz_get_integer(dv, "_RTV", &sc->sc_zone.rtv);
	if (sc->sc_zone.rtv == ATZ_TMP_INVALID)
		sc->sc_zone.rtv = 0;


	acpitz_sane_temp(&sc->sc_zone.tmp);
	acpitz_sane_temp(&sc->sc_zone.crt);
	acpitz_sane_temp(&sc->sc_zone.hot);
	acpitz_sane_temp(&sc->sc_zone.psv);

	if (verbose != 0) {
		int comma = 0;

		aprint_verbose_dev(dv, "");

		if (sc->sc_zone.crt != ATZ_TMP_INVALID) {
			aprint_verbose("critical %s C",
			    acpitz_celcius_string(sc->sc_zone.crt));
			comma = 1;
		}

		if (sc->sc_zone.hot != ATZ_TMP_INVALID) {
			aprint_verbose("%shot %s C", comma ? ", " : "",
			    acpitz_celcius_string(sc->sc_zone.hot));
			comma = 1;
		}

		if (sc->sc_zone.psv != ATZ_TMP_INVALID) {
			aprint_normal("%spassive %s C", comma ? ", " : "",
			    acpitz_celcius_string(sc->sc_zone.psv));
			comma = 1;
		}

		if (valid_levels == 0) {
			sc->sc_flags |= ATZ_F_PASSIVEONLY;

			if (sc->sc_first)
				aprint_verbose("%spassive cooling", comma ?
				    ", " : "");
		}

		aprint_verbose("\n");
	}

	for (i = 0; i < ATZ_NLEVELS; i++)
		acpitz_sane_temp(&sc->sc_zone.ac[i]);

	acpitz_power_off(sc);
	sc->sc_first = 0;
}

static void
acpitz_notify_handler(ACPI_HANDLE hdl, uint32_t notify, void *opaque)
{
	device_t dv = opaque;
	ACPI_OSD_EXEC_CALLBACK func = NULL;
	const char *name;
	ACPI_STATUS rv;

	switch (notify) {
	case ACPI_NOTIFY_ThermalZoneStatusChanged:
		func = acpitz_get_status;
		name = "status check";
		break;
	case ACPI_NOTIFY_ThermalZoneTripPointsChanged:
	case ACPI_NOTIFY_DeviceListsChanged:
		func = acpitz_get_zone_quiet;
		name = "get zone";
		break;
	default:
		aprint_debug_dev(dv,
		    "received unhandled notify message 0x%x\n", notify);
		return;
	}

	KASSERT(func != NULL);

	rv = AcpiOsExecute(OSL_NOTIFY_HANDLER, func, dv);
	if (ACPI_FAILURE(rv))
		aprint_debug_dev(dv, "unable to queue %s\n", name);
}

static void
acpitz_sane_temp(uint32_t *tmp)
{
	/* Sane temperatures are beteen 0 and 150 C */
	if (*tmp < ATZ_ZEROC || *tmp > ATZ_ZEROC + 1500)
		*tmp = ATZ_TMP_INVALID;
}

static int
acpitz_get_integer(device_t dv, const char *cm, uint32_t *val)
{
	struct acpitz_softc *sc = device_private(dv);
	ACPI_STATUS rv;
	ACPI_INTEGER tmp;

	rv = acpi_eval_integer(sc->sc_devnode->ad_handle, cm, &tmp);

	if (ACPI_FAILURE(rv)) {
		*val = ATZ_TMP_INVALID;

		if (rv != AE_NOT_FOUND)
			aprint_debug_dev(dv, "failed to evaluate %s: %s\n",
			    cm, AcpiFormatException(rv));

		return 1;
	}

	*val = tmp;

	return 0;
}

static int
acpitz_get_fanspeed(device_t dv,
    uint32_t *fanmin, uint32_t *fanmax, uint32_t *fancurrent)
{
	struct acpitz_softc *sc = device_private(dv);
	ACPI_STATUS rv;
	ACPI_HANDLE handle;
	ACPI_INTEGER fmin, fmax, fcurr;
	int rc = 0;

	handle = sc->sc_devnode->ad_handle;
	rv = acpi_eval_integer(handle, "FMIN", &fmin);
	if (ACPI_FAILURE(rv)) {
		fmin = ATZ_TMP_INVALID;
		rc = 1;
	}
	rv = acpi_eval_integer(handle, "FMAX", &fmax);
	if (ACPI_FAILURE(rv)) {
		fmax = ATZ_TMP_INVALID;
		rc = 1;
	}
	rv = acpi_eval_integer(handle, "FRSP", &fcurr);
	if (ACPI_FAILURE(rv)) {
		fcurr = ATZ_TMP_INVALID;
		rc = 1;
	}

	if (fanmin)
		*fanmin = fmin;
	if (fanmax)
		*fanmax = fmax;
	if (fancurrent)
		*fancurrent = fcurr;
	return rc;
}

#ifdef notyet
static ACPI_STATUS
acpitz_set_fanspeed(device_t dv, uint32_t fanspeed)
{
	struct acpitz_softc *sc = device_private(dv);
	ACPI_STATUS rv;
	ACPI_HANDLE handle;
	handle = sc->sc_devnode->ad_handle;

	rv = acpi_eval_set_integer(handle, "FSSP", fanspeed);
	if (ACPI_FAILURE(rv))
		aprint_debug_dev(dv, "failed to set fanspeed to %u rpm: %s\n",
			fanspeed, AcpiFormatException(rv));
	return rv;
}
#endif

static void
acpitz_tick(void *opaque)
{
	device_t dv = opaque;
	struct acpitz_softc *sc = device_private(dv);

	AcpiOsExecute(OSL_NOTIFY_HANDLER, acpitz_get_status, dv);

	callout_schedule(&sc->sc_callout, sc->sc_zone.tzp * hz / 10);
}

static void
acpitz_init_envsys(device_t dv)
{
	const int flags = ENVSYS_FMONLIMITS | ENVSYS_FMONNOTSUPP;
	struct acpitz_softc *sc = device_private(dv);

	sc->sc_sme = sysmon_envsys_create();

	sc->sc_sme->sme_cookie = sc;
	sc->sc_sme->sme_name = device_xname(dv);
	sc->sc_sme->sme_flags = SME_DISABLE_REFRESH;
	sc->sc_sme->sme_get_limits = acpitz_get_limits;

	sc->sc_temp_sensor.flags = flags;
	sc->sc_temp_sensor.units = ENVSYS_STEMP;

	(void)strlcpy(sc->sc_temp_sensor.desc,
	    sc->sc_zone.name, sizeof(sc->sc_temp_sensor.desc));

	if (sysmon_envsys_sensor_attach(sc->sc_sme, &sc->sc_temp_sensor))
		goto out;

	if (sc->sc_have_fan != 0) {

		sc->sc_fan_sensor.flags = flags;
		sc->sc_fan_sensor.units = ENVSYS_SFANRPM;

		(void)strlcpy(sc->sc_fan_sensor.desc,
		    "FAN", sizeof(sc->sc_fan_sensor.desc));

		/* Ignore error because fan sensor is optional. */
		(void)sysmon_envsys_sensor_attach(sc->sc_sme,
		    &sc->sc_fan_sensor);
	}

	if (sysmon_envsys_register(sc->sc_sme) == 0)
		return;

out:
	aprint_error_dev(dv, "unable to register with sysmon\n");

	sysmon_envsys_destroy(sc->sc_sme);
	sc->sc_sme = NULL;
}

static void
acpitz_get_limits(struct sysmon_envsys *sme, envsys_data_t *edata,
		  sysmon_envsys_lim_t *limits, uint32_t *props)
{
	struct acpitz_softc *sc = sme->sme_cookie;
	int i;

	switch (edata->units) {
	case ENVSYS_STEMP:
		*props = 0;
		if (sc->sc_zone.hot != ATZ_TMP_INVALID) {
			*props |= PROP_CRITMAX;
			limits->sel_critmax = ATZ2UKELVIN(sc->sc_zone.hot);
		} else if (sc->sc_zone.crt != ATZ_TMP_INVALID) {
			*props |= PROP_CRITMAX;
			limits->sel_critmax = ATZ2UKELVIN(sc->sc_zone.crt);
		}
		for (i = 0; i < ATZ_NLEVELS; i++) {
			if (sc->sc_zone.ac[i] != ATZ_TMP_INVALID) {
				limits->sel_warnmax =
				    ATZ2UKELVIN(sc->sc_zone.ac[i]);
				*props |= PROP_WARNMAX;
				break;
			}
		}
		break;

	case ENVSYS_SFANRPM:
		*props = 0;
		if (sc->sc_zone.fanmin != ATZ_TMP_INVALID) {
			*props |= PROP_WARNMIN;
			limits->sel_warnmin = sc->sc_zone.fanmin;
			sc->sc_fan_sensor.flags |= ENVSYS_FVALID_MIN;
		}
		if (sc->sc_zone.fanmax != ATZ_TMP_INVALID) {
			*props |= PROP_WARNMAX;
			limits->sel_warnmax = sc->sc_zone.fanmax;
			sc->sc_fan_sensor.flags |= ENVSYS_FVALID_MAX;
		}
		break;
	}
}
