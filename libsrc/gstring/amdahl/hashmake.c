#ident "@(#)/usr/src/cmd/spell/hashmake.c	3.1.1.1 6/1/90 00:27:35 - (c) Copyright 1988, 1989, 1990 Amdahl Corporation"
/* Copyright (c) 1984 AT&T */
/*	@(#)hashmake.c	1.1	*/
#include "hash.h"

main()
{
	char word[30];
      /*long h;*/
	hashinit();
	while(gets(word)) {
		printf("%.*lo\n",(HASHWIDTH+2)/3,hash(word));
	}
	return(0);
}
