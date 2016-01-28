:
# @(#)/usr/src/cmd/spell/spell.sh	6.1.1.1 6/1/90 00:29:36 - (c) Copyright 1988, 1989, 1990 Amdahl Corporation
# Copyright (c) 1984 AT&T 
:
#	spell program
# SCCS:		@(#)spell.sh	1.7
# B_SPELL flags, D_SPELL dictionary, F_SPELL input files, H_SPELL history, S_SPELL stop, V_SPELL data for -v
# L_SPELL sed script, I_SPELL -i option to deroff
H_SPELL=${H_SPELL-/usr/lib/spell/spellhist}
V_SPELL=/dev/null
F_SPELL=
B_SPELL=
L_SPELL="sed -e \"/^[.'].*[.'][ 	]*nx[ 	]*\/usr\/lib/d\" -e \"/^[.'].*[.'][ 	]*so[ 	]*\/usr\/lib/d\" -e \"/^[.'][ 	]*so[ 	]*\/usr\/lib/d\" -e \"/^[.'][ 	]*nx[ 	]*\/usr\/lib/d\" "
trap "rm -f /tmp/spell.$$; exit" 0 1 2 13 15
DEROFF=/usr/bin/deroff

set -- `getopt vbxli $* 2> /dev/null`
if [ $? != 0 ]
then
	echo  "Usage : spell [-v] [-b] [-x] [-l] [-i] [+local_file] [files]" >&2
	exit 2
fi
for A in $*
do
	case $A in
	-v)	if /bin/pdp11
			then	echo -v option not supported on pdp11 >&2
				EXIT_SPELL=exit
		else	B_SPELL="$B_SPELL -v"
			V_SPELL=/tmp/spell.$$
		fi ;;
	-a)	: ;;
	-b) 	D_SPELL=${D_SPELL-/usr/lib/spell/hlistb}
		B_SPELL="$B_SPELL -b" ;;
	-x)	B_SPELL="$B_SPELL -x" ;;
	-l)	L="cat" ;;
	+*)	if [ "$FIRSTPLUS" = "+" ]
			then	echo "multiple + options in spell, all but the last are ignored" >&2
		fi;
		FIRSTPLUS="$FIRSTPLUS"+
		if  LOCAL=`expr $A : '+\(.*\)' 2>/dev/null`;
		then if test ! -r $LOCAL;
			then echo "spell: cannot read $LOCAL" >&2; EXIT_SPELL=exit;
		     fi
		else echo "spell: cannot identify local spell file" >&2; EXIT_SPELL=exit;
		fi ;;
	-i)	I_SPELL="-i" ;;
	--)	;;
	*)	if [ -r $A ]
		then
			F_SPELL="$F_SPELL $A"
		else
			echo "spell: cannot read $A" >&2
			EXIT_SPELL="exit 2"
		fi
	esac
done
${EXIT_SPELL-:}
cat $F_SPELL | eval $L_SPELL |\
 ${DEROFF} -w $I_SPELL |\
 sort -u +0 |\
 /usr/lib/spell/spellprog ${S_SPELL-/usr/lib/spell/hstop} 1 |\
 /usr/lib/spell/spellprog ${D_SPELL-/usr/lib/spell/hlista} $V_SPELL $B_SPELL |\
 comm -23 - ${LOCAL-/dev/null} |\
 tee -a $H_SPELL 2>/dev/null
# If possible, try to make history file accessible by the world
who am i 2>/dev/null >>$H_SPELL 
chgrp bin $H_SPELL 2>/dev/null
chown bin $H_SPELL 2>/dev/null
chmod 666 $H_SPELL 2>/dev/null
# If not possible, warn the user.  Nothing else to do!
if [ ! -f $H_SPELL -o ! -w $H_SPELL ]
then
	echo "$0: WARNING: $H_SPELL not writeable or does not exist"
fi
case $V_SPELL in
/dev/null)	exit
esac
sed '/^\./d' $V_SPELL | sort -u +1f +0
