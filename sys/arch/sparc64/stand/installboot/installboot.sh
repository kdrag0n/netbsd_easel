#! /bin/sh
#
#	$NetBSD: installboot.sh,v 1.2 1998/11/24 00:03:19 mrg Exp $
#
# Copyright (c) 1998 Matthew R. Green
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

usage='installboot [-vd] <bootblk> <disk>'

verbose=0
debug=0

while [ $# -gt 0 ]; do
	case "$1" in
		-d)
			debug=1
			shift
			;;
		-v)
			verbose=1
			shift
			;;
		--)
			shift
			break
			;;
		-*)
			echo $usage
			exit 1
			;;
		*)
			break
			;;
	esac
done

blk=$1
disk=$2
if [ -z "$blk" -o -z "$disk" ]; then
	echo $usage
	exit 1
fi

cmd="dd if=$blk of=$disk bs=512 count=15 conv=notrunc seek=1"

if [ $verbose = 1 ]; then
	echo $cmd
fi

if [ $debug = 0 ]; then
	$cmd
fi
