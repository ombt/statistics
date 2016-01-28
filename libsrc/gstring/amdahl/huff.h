//
// Copyright (C) 2010, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ident "@(#)/usr/src/cmd/spell/huff.h	1.3.1.1 6/1/90 00:28:23 - (c) Copyright 1988, 1989, 1990 Amdahl Corporation"
/* Copyright (c) 1984 AT&T */
/*	@(#)huff.h	1.1	*/
extern struct huff {
	long xn;
	int xw;
	long xc;
	long xcq;	/* (c,0) */
	long xcs;	/* c left justified */
	long xqcs;	/* (q-1,c,q) left justified */
	long xv0;
} huffcode;
#define n huffcode.xn
#define w huffcode.xw
#define c huffcode.xc
#define cq huffcode.xcq
#define cs huffcode.xcs
#define qcs huffcode.xqcs
#define v0 huffcode.xv0
