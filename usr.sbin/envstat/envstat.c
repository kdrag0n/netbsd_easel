/*	$NetBSD: envstat.c,v 1.9 2003/01/05 01:56:42 chris Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Bill Squier.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: envstat.c,v 1.9 2003/01/05 01:56:42 chris Exp $");
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <paths.h>

#include <sys/envsys.h>
#include <sys/ioctl.h>

const char E_CANTENUM[] = "cannot enumerate sensors";

int main(int, char **);
void listsensors(envsys_basic_info_t *, int);
int numsensors(int);
int fillsensors(int, envsys_tre_data_t *, envsys_basic_info_t *, int);
int longestname(envsys_basic_info_t *, int);
int marksensors(envsys_basic_info_t *, int *, char *, int);
int strtosnum(envsys_basic_info_t *, const char *, int);
void header(unsigned, int, envsys_basic_info_t *, const int * const, int);
void values(unsigned, int, envsys_tre_data_t *, const int * const, int);
void usage(void);

int rflag = 0;

static const char *unit_str[] = {"degC", "RPM", "VAC", "V", "Ohms", "W",
				 "A", "Wh", "Ah", "bool", "Unk"};

int
main(int argc, char **argv)
{
	int c, fd, ns, ls, celsius;
	unsigned int interval, width, headrep, headcnt;
	envsys_tre_data_t *etds;
	envsys_basic_info_t *ebis;
	int *cetds;
	char *sensors;
	const char *dev;

	fd = -1;
	ls = 0;
	celsius = 1;
	interval = 0;
	width = 0;
	sensors = NULL;
	headrep = 22;

	while ((c = getopt(argc, argv, "cfi:ln:rs:w:r")) != -1) {
		switch(c) {
		case 'r':
			rflag= 1;
			break;
		case 'i':	/* wait time between displays */
			interval = atoi(optarg);
			break;
		case 'w':	/* minimum column width */
			width = atoi(optarg);
			break;
		case 'l':	/* list sensor names */
			ls = 1;
			break;
		case 'n':	/* repeat header every headrep lines */
			headrep = atoi(optarg);
			break;
		case 's':	/* restrict display to named sensors */
			sensors = (char *)malloc(strlen(optarg) + 1);
			if (sensors == NULL)
				exit(1);
			strcpy(sensors, optarg);
			break;
		case 'f':	/* display temp in degF */
			celsius = 0;
			break;
		case '?':
		default:
			usage();
			/* NOTREACHED */
		}
	}

	if (optind < argc)
		dev = argv[optind];
	else
		dev = _PATH_SYSMON;

	if ((fd = open(dev, O_RDONLY)) == -1)
		err(1, "unable to open %s", dev);

	/*
	 * Determine number of sensors, allocate and fill arrays with
	 * initial information.  Determine column width
	 */
	if ((ns = numsensors(fd)) <= 0)
		errx(1, E_CANTENUM);

	cetds = (int *)malloc(ns * sizeof(int));
	etds = (envsys_tre_data_t *)malloc(ns * sizeof(envsys_tre_data_t));
	ebis = (envsys_basic_info_t *)malloc(ns * sizeof(envsys_basic_info_t));

	if ((cetds == NULL) || (etds == NULL) || (ebis == NULL))
		errx(1, "cannot allocate memory");

	if (fillsensors(fd, etds, ebis, ns) == -1)
		errx(1, E_CANTENUM);

	if (ls) {
		listsensors(ebis, ns);
		exit(0);
	}


#define MAX(x, y)  (((x) > (y)) ? (x) : (y))
	if (!width) {
		width = longestname(ebis, ns);
		width = MAX(((79 - ns) / ns), width);
	}

	/* Mark which sensor numbers are to be displayed */
	if (marksensors(ebis, cetds, sensors, ns) == -1)
		exit(1);

	if (rflag) {
		int i;

		for (i = 0 ; i < ns ; i++) {
			if (ebis[i].units == ENVSYS_INDICATOR) {
				if (etds[i].cur.data_s) {
					printf("%*.*s\n",
					    (int)width,
					    (int)width,
					    ebis[i].desc);
				}
				continue;
			}

			if (ebis[i].units == ENVSYS_INTEGER) {
				printf("%*.*s:", (int)width, (int)width,
				       ebis[i].desc);
				printf(" %10d\n", etds[i].cur.data_s);
				continue;
			}

			printf("%*.*s:", (int)width, (int)width, ebis[i].desc);
			/* different units need some magic */
			switch (ebis[i].units)
			{
			     	case ENVSYS_INDICATOR:
					printf(" %10s", etds[i].cur.data_us ? 
					    "ON" : "OFF");
					break;
				case ENVSYS_STEMP:
					{
					     	double temp =
						    (etds[i].cur.data_s / 1000000.0)
						    - 273.15;
						if (celsius)
							printf(" %10.3f degC",
							    temp);
						else
						{
							temp = (9.0 / 5.0) * temp + 32.0;
							printf(" %10.3f degF",
							    temp);
						}
					}
					break;
				case ENVSYS_SFANRPM:
					printf(" %10u RPM",
					    etds[i].cur.data_us);
					break;
				default:
					printf(" %10.3f %s",
					    etds[i].cur.data_s / 1000000.0,
					    unit_str[ebis[i].units]);
					break;
			}
			if (etds[i].validflags & ENVSYS_FFRACVALID) {
				printf(" (%5.2f%%)",
				    (etds[i].cur.data_s * 100.0) /
				    etds[i].max.data_s);
			}

			printf("\n");
		}
		exit(1);
	}



	/* If we didn't specify an interval value, print the sensors once */
	if (!interval) {
		if (headrep)
			header(width, celsius, ebis, cetds, ns);
		values(width, celsius, etds, cetds, ns);

		exit (0);
	}

	headcnt = 0;
	if (headrep)
		header(width, celsius, ebis, cetds, ns);

        for (;;) {
		values(width, celsius, etds, cetds, ns);
		if (headrep && (++headcnt == headrep)) {
			headcnt = 0;
			header(width, celsius, ebis, cetds, ns);
		}

		sleep(interval);

		if (fillsensors(fd, etds, ebis, ns) == -1)
			errx(1, E_CANTENUM);
	}

	/* NOTREACHED */
	return (0);
}


/*
 * pre:  cetds[i] != 0 iff sensor i should appear in the output
 * post: a column header line is displayed on stdout
 */
void
header(unsigned width, int celsius, envsys_basic_info_t *ebis,
       const int * const cetds, int ns)
{
	int i;
	const char *s;

	/* sensor names */
	for (i = 0; i < ns; ++i)
		if (cetds[i])
			printf(" %*.*s", (int)width, (int)width, ebis[i].desc);

	printf("\n");

	/* units */
	for (i = 0; i < ns; ++i)
		if (cetds[i]) {
			if ((ebis[i].units == ENVSYS_STEMP) &&
			    !celsius)
				s = "degF";
			else if (ebis[i].units >= ENVSYS_NSENSORS)
				s = unit_str[ENVSYS_NSENSORS];
			else
				s = unit_str[ebis[i].units];

			printf(" %*.*s", (int)width, (int)width, s);
		}
	printf("\n");
}

void
values(unsigned width, int celsius, envsys_tre_data_t *etds,
       const int * const cetds, int ns)
{
	int i;
	double temp;

	for (i = 0; i < ns; ++i)
		if (cetds[i]) {

			/* * sensors without valid data */
			if ((etds[i].validflags & ENVSYS_FCURVALID) == 0) {
				printf(" %*.*s", (int)width, (int)width, "*");
				continue;
			}

			switch(etds[i].units) {
			case ENVSYS_INDICATOR:
				printf(" %*.*s", (int)width, (int)width,
				    etds[i].cur.data_us ? "ON" : "OFF");
				break;
			case ENVSYS_STEMP:
				temp = (etds[i].cur.data_us / 1000000.0) -
				    273.15;
				if (!celsius)
					temp = (9.0 / 5.0) * temp + 32.0;
				printf(" %*.2f", width, temp);
				break;
			case ENVSYS_SFANRPM:
				printf(" %*u", width, etds[i].cur.data_us);
				break;
			default:
				printf(" %*.2f", width, etds[i].cur.data_s /
				       1000000.0);
				break;
			}
		}
	printf("\n");
}


/*
 * post: displays usage on stderr
 */
void
usage(void)
{

	fprintf(stderr, "usage: %s [-c] [-s s1,s2,...]", getprogname());
	fprintf(stderr, " [-i interval] [-n headrep] [-w width] [device]\n");
	fprintf(stderr, "       envstat -l [device]\n");
	exit(1);
}


/*
 * post: a list of sensor names supported by the device is displayed on stdout
 */
void
listsensors(envsys_basic_info_t *ebis, int ns)
{
	int i;

	for (i = 0; i < ns; ++i)
		if (ebis[i].validflags & ENVSYS_FVALID)
			printf("%s\n", ebis[i].desc);
}


/*
 * pre:  fd contains a valid file descriptor of an envsys(4) supporting device
 * post: returns the number of valid sensors provided by the device
 *       or -1 on error
 */
int
numsensors(int fd)
{
	int count = 0, valid = 1;
	envsys_tre_data_t etd;
	etd.sensor = 0;

	while (valid) {
		if (ioctl(fd, ENVSYS_GTREDATA, &etd) == -1) {
			fprintf(stderr, E_CANTENUM);
			exit(1);
		}

		valid = etd.validflags & ENVSYS_FVALID;
		if (valid)
			++count;

		++etd.sensor;
	}

	return count;
}

/*
 * pre:  fd contains a valid file descriptor of an envsys(4) supporting device
 *       && ns is the number of sensors
 *       && etds and ebis are arrays of sufficient size
 * post: returns 0 and etds and ebis arrays are filled with sensor info
 *       or returns -1 on failure
 */
int
fillsensors(int fd, envsys_tre_data_t *etds, envsys_basic_info_t *ebis, int ns)
{
	int i;

	for (i = 0; i < ns; ++i) {
		ebis[i].sensor = i;
		if (ioctl(fd, ENVSYS_GTREINFO, &ebis[i]) == -1)
			return -1;

		etds[i].sensor = i;
		if (ioctl(fd, ENVSYS_GTREDATA, &etds[i]) == -1)
			return -1;
	}

	return 0;
}


/*
 * post: returns the strlen() of the longest sensor name
 */
int
longestname(envsys_basic_info_t *ebis, int ns)
{
	int i, maxlen, cur;

	maxlen = 0;

	for (i = 0; i < ns; ++i) {
		cur = strlen(ebis[i].desc);
		if (cur > maxlen)
			maxlen = cur;
	}

	return maxlen;
}

/*
 * post: returns 0 and cetds[i] != 0 iff sensor i should appear in the output
 *       or returns -1
 */
int
marksensors(envsys_basic_info_t *ebis, int *cetds, char *sensors, int ns)
{
	int i;
	char *s;

	if (sensors == NULL) {
		/* No sensors specified, include them all */
		for (i = 0; i < ns; ++i)
			cetds[i] = 1;

		return 0;
	}

	/* Assume no sensors in display */
	memset(cetds, 0, ns * sizeof(int));

	s = strtok(sensors, ",");
	while (s != NULL) {
		if ((i = strtosnum(ebis, s, ns)) != -1)
			cetds[i] = 1;
		else {
			fprintf(stderr, "envstat: unknown sensor %s\n", s);
			return (-1);
		}

		s = strtok(NULL, ",");
	}

	/* Check if we have at least one sensor selected for output */
	for (i = 0; i < ns; ++i)
		if (cetds[i] == 1)
			return (0);

	fprintf(stderr, "envstat: no sensors selected for display\n");
	return (-1);
}


/*
 * returns -1 if s is not a valid sensor name for the device
 *       or the sensor number of a sensor which has that name
 */
int
strtosnum(envsys_basic_info_t *ebis, const char *s, int ns)
{
	int i;

	for (i = 0; i < ns; ++i) {
		if((ebis[i].validflags & ENVSYS_FVALID) &&
		   !strcmp(s, ebis[i].desc))
			return ebis[i].sensor;
	}

	return -1;
}
