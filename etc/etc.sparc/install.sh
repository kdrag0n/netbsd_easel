#!/bin/sh
# $Id: install.sh,v 1.3 1994/07/27 05:26:20 deraadt Exp $
umask 0
cat ./bin.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./dev.cpio.gz | gzip -d | (cd /mnt; cpio -iduv)
cat ./etc.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./sbin.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.bin.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.games.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.include.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.lib.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.libexec.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.misc.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.sbin.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./usr.share.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cat ./var.tar.gz | gzip -d | (cd /mnt; tar xvpf -)
cp ./netbsd /mnt/netbsd
