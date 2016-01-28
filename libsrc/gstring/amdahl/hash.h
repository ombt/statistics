//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ident "@(#)/usr/src/cmd/spell/hash.h	3.1.1.1 6/1/90 00:26:16 - (c) Copyright 1988, 1989, 1990 Amdahl Corporation"
/* Copyright (c) 1984 AT&T */
/*	@(#)hash.h	1.1	*/
#define HASHWIDTH 27
#define HASHSIZE 134217689L	/*prime under 2^HASHWIDTH*/
#define INDEXWIDTH 9
#define INDEXSIZE (1<<INDEXWIDTH)
#define NI (INDEXSIZE+1)
#define ND ((25750/2)*sizeof(*table))
#define BYTE 8

extern unsigned *table;
extern int index[];	/*into dif table based on hi hash bits*/

extern unsigned long hash();
