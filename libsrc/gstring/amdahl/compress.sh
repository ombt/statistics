:
# @(#)/usr/src/cmd/spell/compress.sh	1.3.1.1 6/1/90 00:25:30 - (c) Copyright 1988, 1989, 1990 Amdahl Corporation
# Copyright (c) 1984 AT&T
#	compress - compress the spell program log
#  SCCS:	@(#)compress.sh	1.1

trap 'rm -f /usr/tmp/spellhist;exit' 1 2 3 15
echo "COMPRESSED `date`" > /usr/tmp/spellhist
grep -v ' ' /usr/lib/spell/spellhist | sort -fud >> /usr/tmp/spellhist
cp /usr/tmp/spellhist /usr/lib/spell
rm -f /usr/tmp/spellhist
