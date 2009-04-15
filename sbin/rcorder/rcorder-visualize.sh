#!/bin/sh
# $NetBSD: rcorder-visualize.sh,v 1.2 2009/04/15 22:38:26 joerg Exp $
#
# Writte by Joerg Sonnenberger.  You may freely use and redistribute
# this script.
#
# Simple script to show the dependency graph for rc scripts.
# Output is in the dot(1) language and can be rendered using
#	sh rcorder-visualize | dot -T svg -o rcorder.svg
# dot(1) can be found in graphics/graphviz in pkgsrc.

#rc_files=/etc/rc.d/*
rc_files=~/data/NetBSD/clean/src/etc/rc.d/*

{
echo ' digraph {'
for f in $rc_files; do
< $f awk '
/# PROVIDE: /	{ provide = $3 }
/# REQUIRE: /	{ for (i = 3; i <= NF; i++) requires[$i] = $i }
/# BEFORE: /	{ for (i = 3; i <= NF; i++) befores[$i] = $i }

END {
	print "    \"" provide "\";"
	for (x in requires) print "    \"" provide "\"->\"" x "\";"
	for (x in befores) print "    \"" x "\"->\"" provide "\";"
}
'
done
echo '}'
}
