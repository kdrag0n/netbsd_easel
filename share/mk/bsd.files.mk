#	$NetBSD: bsd.files.mk,v 1.39 2008/08/25 08:11:04 lukem Exp $

.if !defined(_BSD_FILES_MK_)
_BSD_FILES_MK_=1

.include <bsd.init.mk>

.if !target(__fileinstall)
##### Basic targets
realinstall:	filesinstall

##### Default values
FILESDIR?=	${BINDIR}
FILESOWN?=	${BINOWN}
FILESGRP?=	${BINGRP}
FILESMODE?=	${NONBINMODE}

##### Install rules
filesinstall::	# ensure existence
.PHONY:		filesinstall

configfilesinstall:: .PHONY

__fileinstall: .USE
	${_MKTARGET_INSTALL}
	${INSTALL_FILE} \
	    -o ${FILESOWN_${.ALLSRC:T}:U${FILESOWN}} \
	    -g ${FILESGRP_${.ALLSRC:T}:U${FILESGRP}} \
	    -m ${FILESMODE_${.ALLSRC:T}:U${FILESMODE}} \
	    ${SYSPKGTAG} ${.ALLSRC} ${.TARGET}

.endif # !target(__fileinstall)


.for F in ${FILES:O:u}
_FDIR:=		${FILESDIR_${F}:U${FILESDIR}}		# dir override
_FNAME:=	${FILESNAME_${F}:U${FILESNAME:U${F:T}}}	# name override
_F:=		${DESTDIR}${_FDIR}/${_FNAME}		# installed path

.if ${MKUPDATE} == "no"
${_F}!		${F} __fileinstall			# install rule
.if !defined(BUILD) && !make(all) && !make(${F})
${_F}!		.MADE					# no build at install
.endif
.else
${_F}:		${F} __fileinstall			# install rule
.if !defined(BUILD) && !make(all) && !make(${F})
${_F}:		.MADE					# no build at install
.endif
.endif

filesinstall::	${_F}
.PRECIOUS: 	${_F}					# keep if install fails
.endfor


# 
# CONFIGFILES
#
configinstall:	configfilesinstall

.for F in ${CONFIGFILES:O:u}
_FDIR:=		${FILESDIR_${F}:U${FILESDIR}}		# dir override
_FNAME:=	${FILESNAME_${F}:U${FILESNAME:U${F:T}}}	# name override
_F:=		${DESTDIR}${_FDIR}/${_FNAME}		# installed path

.if ${MKUPDATE} == "no"
${_F}!		${F} __fileinstall	# install rule
.if !defined(BUILD) && !make(all) && !make(${F})
${_F}!		.MADE					# no build at install
.endif
.else
${_F}:		${F} __fileinstall	# install rule
.if !defined(BUILD) && !make(all) && !make(${F})
${_F}:		.MADE					# no build at install
.endif
.endif

configfilesinstall::	${_F}
.PRECIOUS: 	${_F}					# keep if install fails
.endfor

.undef _FDIR
.undef _FNAME
.undef _F


#
# BUILDSYMLINKS
#
.if defined(BUILDSYMLINKS)					# {

.for _SL _TL in ${BUILDSYMLINKS}
BUILDSYMLINKS.s+=	${_SL}
BUILDSYMLINKS.t+=	${_TL}
${_TL}: ${_SL}
	${_MKMSG} "symlink " ${.CURDIR:T}/${.TARGET}
	rm -f ${.TARGET}
	ln -s ${.ALLSRC} ${.TARGET}
.endfor

realall: ${BUILDSYMLINKS.t}

cleandir: cleanbuildsymlinks
cleanbuildsymlinks: .PHONY
	rm -f ${BUILDSYMLINKS.t}

.endif								# }

#
# .uue -> "" handling (i.e. decode a given binary/object)
#
# UUDECODE_FILES -	List of files which are stored in the source tree
#			as <file>.uue and should be uudecoded.
#
# UUDECODE_FILES_RENAME_fn - For this file, rename its output to the provided
#			     name (handled via -p and redirecting stdout)

.if defined(UUDECODE_FILES)					# {
.SUFFIXES:	.uue

.uue:
	${_MKTARGET_CREATE}
	rm -f ${.TARGET} ${.TARGET}.tmp
	${TOOL_UUDECODE} -p ${.IMPSRC} > ${.TARGET}.tmp \
	    && mv ${.TARGET}.tmp ${UUDECODE_FILES_RENAME_${.TARGET}:U${.TARGET}}

realall: ${UUDECODE_FILES}

CLEANUUDECODE_FILES=${UUDECODE_FILES} ${UUDECODE_FILES:=.tmp}
.for i in ${UUDECODE_FILES}
CLEANUUDECODE_FILES+=${UUDECODE_FILES_RENAME_${i}}
.endfor

clean: cleanuudecodefiles
cleanuudecodefiles: .PHONY
	rm -f ${CLEANUUDECODE_FILES}
.endif								# }

##### Pull in related .mk logic
.include <bsd.obj.mk>
.include <bsd.sys.mk>

.endif	# !defined(_BSD_FILES_MK_)
