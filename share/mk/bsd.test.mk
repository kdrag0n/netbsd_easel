# $NetBSD: bsd.test.mk,v 1.9 2010/06/18 10:10:57 jmmv Exp $
#

.include <bsd.init.mk>

TESTSBASE=	/usr/tests

_TESTS=		# empty

.if defined(TESTS_C)
PROGS+=		${TESTS_C}
LDADD+=		-latf-c
.  for _T in ${TESTS_C}
BINDIR.${_T}=	${TESTSDIR}
MAN.${_T}?=	# empty
_TESTS+=	${_T}
.  endfor
.endif

.if defined(TESTS_CXX)
PROGS_CXX+=	${TESTS_CXX}
LDADD+=		-latf-c++ -latf-c
.  for _T in ${TESTS_CXX}
BINDIR.${_T}=	${TESTSDIR}
MAN.${_T}?=	# empty
_TESTS+=	${_T}
.  endfor
.endif

.if defined(TESTS_SH)

.  for _T in ${TESTS_SH}
SCRIPTS+=		${_T}
SCRIPTSDIR_${_T}=	${TESTSDIR}

_TESTS+=		${_T}
CLEANFILES+=		${_T} ${_T}.tmp

TESTS_SH_SRC_${_T}?=	${_T}.sh
${_T}: ${TESTS_SH_SRC_${_T}}
	${_MKTARGET_BUILD}
	echo '#! /usr/bin/atf-sh' >${.TARGET}.tmp
	cat ${.ALLSRC} >>${.TARGET}.tmp
	chmod +x ${.TARGET}.tmp
	mv ${.TARGET}.tmp ${.TARGET}
.  endfor
.endif

.if !defined(NOATFFILE)
FILES+=			Atffile
FILESDIR_Atffile=	${TESTSDIR}
.include <bsd.files.mk>
.endif

.if !empty(SCRIPTS) || !empty(PROGS) || !empty(PROGS_CXX)
.  include <bsd.prog.mk>
.endif

#
# Definition of the "make test" target and supporting variables.
#
# This target, by necessity, can only work for native builds (i.e. a NetBSD
# host building a release for the same system).  The target runs ATF, which is
# not in the toolchain, and the tests execute code built for the target host.
#
# Due to the dependencies of the binaries built by the source tree and how they
# are used by tests, it is highly possible for a execution of "make test" to
# report bogus results unless the new binaries are put in place.
#

TESTS_PATH += ${DESTDIR}/bin ${DESTDIR}/sbin ${DESTDIR}/usr/bin ${DESTDIR}/usr/sbin
TESTS_LD_LIBRARY_PATH += ${DESTDIR}/lib ${DESTDIR}/usr/lib

TESTS_ENV += LD_LIBRARY_PATH=${TESTS_LD_LIBRARY_PATH:tW:S/ /:/g}
TESTS_ENV += PATH=${TESTS_PATH:tW:S/ /:/g}

_TESTS_FIFO = ${.OBJDIR}/atf-run.fifo
_TESTS_LOG = ${.OBJDIR}/atf-run.log
CLEANFILES += ${_TESTS_FIFO} ${_TESTS_LOG}

.PHONY: test
.if ${TESTSDIR} == ${TESTSBASE}
# Forbid this case.  It is likely to cause false positives/negatives and it
# does not cover all the tests (e.g. it misses testing software in external).
test:
	@echo "*** Sorry, you cannot use make test from src/tests.  Install the"
	@echo "*** tests into their final location and run them from /usr/tests"
	@false
.else
test:
	@echo "*** WARNING: make test is experimental"
	@echo "***"
	@echo "*** Using this test does not preclude you from running the tests"
	@echo "*** installed in /usr/tests.  This test run may raise false"
	@echo "*** positives and/or false negatives."
	@echo
	@cd ${DESTDIR}${TESTSDIR}; \
	mkfifo ${_TESTS_FIFO}; \
	cat ${_TESTS_FIFO} | tee ${_TESTS_LOG} | \
	    ${TESTS_ENV} ${DESTDIR}/usr/bin/atf-report & \
	${TESTS_ENV} ${DESTDIR}/usr/bin/atf-run >>${_TESTS_FIFO}; \
	result=$${?}; \
	wait; \
	rm -f ${_TESTS_FIFO}; \
	echo; \
	echo "*** The verbatim output of atf-run has been saved to ${_TESTS_LOG}"; \
	echo "*** Once again, note that "make test" is unsupported."; \
	test $${result} -eq 0
.endif
