#	$NetBSD: runlist.sh,v 1.1 2002/02/11 17:49:42 thorpej Exp $

if [ "X$1" = "X-d" ]; then
	SHELLCMD=cat
	shift
else
	SHELLCMD="sh -e"
fi

( while [ "X$1" != "X" ]; do
	cat $1
	shift
done ) | awk -f ${TOPDIR}/src/list2sh.awk | ${SHELLCMD}
