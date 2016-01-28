#ident "@(#)/usr/src/cmd/spell/spell.mk	6.2.1.1 6/1/90 00:29:12 - (c) Copyright 1988, 1989, 1990 Amdahl Corporation"
# Copyright (c) 1984 AT&T
#	spell make file
#	SCCS:  @(#)spell.mk	1.7

# _SH_ is used by 3B5 View-path environment

ROOT =
OL = $(ROOT)/
SL = $(ROOT)/usr/src/cmd
RDIR = ${SL}/spell
INSTALL_ONLY = :
INS = ${INSTALL_ONLY} /etc/install -f
REL = current
CSID = -r`gsid spellcode ${REL}`
DSID = -r`gsid spelldata ${REL}`
SHSID = -r`gsid spell.sh ${REL}`
CMPRSID = -r`gsid compress.sh ${REL}`
MKSID = -r`gsid spell.mk ${REL}`
BIN = P108
LIST = opr -ttx -b${BIN}
CINSDIR = ${OL}usr/bin
PINSDIR = ${OL}usr/lib/spell
DINSDIR = ${OL}usr/dict
IFLAG = -i
SMFLAG =
CFLAGS = -O
SFILES = spellprog.c spellin.c
DFILES = american british local list extra stop
MAKE = make
SHELL=/bin/sh

compile all: spell hlista hlistb hstop spellin spellin1 spellprog hashmake \
	hashmk1 hashcheck compress dict
	:

spell:	spellprog spell.sh
	cat spell.sh > spell
	> $(PINSDIR)/spellhist
	chmod 666 $(PINSDIR)/spellhist
	chown bin $(PINSDIR)/spellhist
	chgrp bin $(PINSDIR)/spellhist
	$(INS) $(CINSDIR) spell

spelldir:
	-${INSTALL_ONLY} mkdir ${PINSDIR}
	${INSTALL_ONLY} chmod 775 ${PINSDIR}

compress:  spelldir compress.sh
	cat compress.sh > compress
	${INS} ${PINSDIR} compress

spellprog: spelldir spellprog.o hash.o hashlook.o huff.o malloc.o
	$(CC) $(IFLAG) ${SMFLAG} ${FFLAG} -O spellprog.o hash.o hashlook.o huff.o malloc.o -o spellprog
	$(INS) $(PINSDIR) spellprog
	
spellprog.o: spellprog.c 
	$(CC) ${CFLAGS} -c -C spellprog.c
hashlook.o: hashlook.c hash.h huff.h
	$(CC) ${CFLAGS} -c -C hashlook.c
spellin1: spelldir spellin.o huff.o
	cc $(IFLAG) ${SMFLAG} ${FFLAG} -O spellin.o huff.o -o spellin1

spellin: spelldir spellin.o huff.o
	$(CC) $(IFLAG) ${SMFLAG} ${FFLAG} -O spellin.o huff.o -o spellin
	${INS} ${PINSDIR} spellin

hashcheck: spelldir hashcheck.o hash.o huff.o
	$(CC) $(IFLAG) ${SMFLAG} ${FFLAG} -O hashcheck.o hash.o huff.o -o hashcheck
	${INS} ${PINSDIR} hashcheck

hashmk1: spelldir hashmake.o hash.o
	cc $(IFLAG) ${SMFLAG} ${FFLAG} -O hashmake.o hash.o -o hashmk1
	
hashmake.c: hashmake.c hash.h 
	cc -c -C hashmake.c

hashmake: spelldir hashmake.o hash.o
	$(CC) $(IFLAG) ${SMFLAG} ${FFLAG} -O hashmake.o hash.o -o hashmake
	${INS} ${PINSDIR} hashmake

alldata: hlista hlistb hstop
	rm htemp1

htemp1:	list local extra hashmk1
	cat list local extra | $(_SH_) ./hashmk1 >htemp1

hlista: spelldir american hashmake hashmk1 spellin spellin1 htemp1
	$(_SH_) ./hashmk1 <american |sort -u - htemp1 >htemp2
	$(_SH_) ./spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlista
	$(INS) $(PINSDIR)  hlista
	rm htemp2

hlistb: spelldir british hashmk1 spellin1 htemp1
	$(_SH_) ./hashmk1 <british |sort -u - htemp1 >htemp2
	$(_SH_) ./spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hlistb
	$(INS) $(PINSDIR)  hlistb
	rm htemp2


hstop:	spelldir stop spellin1 hashmk1
	$(_SH_) ./hashmk1 <stop | sort -u >htemp2
	$(_SH_) ./spellin1 `wc htemp2|sed -n 's/\([^ ]\) .*/\1/p' ` <htemp2 >hstop
	$(INS) $(PINSDIR)  hstop
	rm htemp2

dict:	american british extra stop list local
	rm -f words
	ln list words
	sort -f -d -o all american british extra words local
	$(INS) ${DINSDIR} all
	$(INS) ${DINSDIR} words
	$(INS) ${DINSDIR} american 
	$(INS) ${DINSDIR} british 
	$(INS) ${DINSDIR} extra 
	$(INS) ${DINSDIR} local
	$(INS) ${DINSDIR} stop

install:  ;  ${MAKE} -f spell.mk INSTALL_ONLY= OL=${OL} all
inssh:    ;  ${MAKE} -f spell.mk INSTALL_ONLY= OL=${OL} spell
inscomp:  ;  ${MAKE} -f spell.mk INSTALL_ONLY= OL=${OL} compress
inscode:  ;  ${MAKE} -f spell.mk INSTALL_ONLY= spell OL=${OL}
insdata:  ;  ${MAKE} -f spell.mk INSTALL_ONLY= alldata OL=${OL}

listing:  ;  pr spell.mk spell.sh compress.sh ${SFILES} ${DFILES} | ${LIST}
listmk:   ;  pr spell.mk | ${LIST}
listsh:	  ;  pr spell.sh | ${LIST}
listcomp: ;  pr compress.sh | ${LIST}
listcode: ;  pr ${SFILES} | ${LIST}
listdata: ;  pr ${DFILES} | ${LIST}

build:  bldmk bldsh bldcomp bldcode blddata
	:
bldcode:  ;  get -p ${CSID} s.spell.src ${REWIRE} | ntar -d ${RDIR} -g
blddata:  ;  get -p ${DSID} s.spell.data | ntar -d ${RDIR} -g
bldsh:	  ;  get -p ${SHSID} s.spell.sh ${REWIRE} > ${RDIR}/spell.sh
bldcomp:  ;  get -p ${CMPRSID} s.compress.sh ${REWIRE} > ${RDIR}/compress.sh
bldmk:    ;  get -p ${MKSID} s.spell.mk > ${RDIR}/spell.mk

edit:	sedit dedit mkedit shedit compedit
	:
sedit:	;  get -p -e s.spell.src | ntar -g
dedit:	;  get -p -e s.spell.data | ntar -g
shedit:	;  get -e s.spell.sh
compedit: ; get -e s.compress.sh

delta:	sdelta ddelta mkdelta shdelta compdelta
	:
sdelta:
	ntar -p ${SFILES} > spell.src
	delta s.spell.src
	rm -f ${SFILES}
ddelta:
	ntar -p ${DFILES} > spell.data
	delta s.spell.data
	rm -f ${DFILES}
shdelta:
	delta s.spell.sh
compdelta: ; delta s.compress.sh

mkedit:	;  get -e s.spell.mk
mkdelta: ; delta s.spell.mk

clean:
	rm -f *.o

clobber: clean shclobber compclobber
	-rm -f spell spellprog spellin hashmake hlist* hstop spellin1 hashmk1
	-rm -f htemp1 htemp2 hashcheck
	-rm -f compress descript
	-rm -f all words
shclobber: ; rm -f spell
compclobber: ; rm -f compress

delete:	clobber shdelete compdelete
	rm -f ${SFILES} ${DFILES}
shdelete: shclobber
	rm -f spell.sh
compdelete: compclobber
	rm -f compress.sh
