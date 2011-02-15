#!/bin/sh
#
# Copyright (C) 2010  Internet Systems Consortium, Inc. ("ISC")
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

# Id: clean.sh,v 1.2 2010-06-22 03:58:36 marka Exp

rm -f ns1/K*
rm -f ns1/*.signed
rm -f ns1/signer.err
rm -f ns1/dsset-*
rm -f ns1/named.run
rm -f ns1/named.memstats

rm -f ns2/named.run
rm -f ns2/named.memstats

rm -f ns3/named.run
rm -f ns3/named.memstats

rm -f ns4/K*
rm -f ns4/*.signed
rm -f ns4/signer.err
rm -f ns4/dsset-*
rm -f ns4/named.run
rm -f ns4/named.memstats

rm -f random.data
rm -f dig.out.*
