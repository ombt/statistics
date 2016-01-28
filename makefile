#
# Copyright (C) 2016, OMBT LLC and Mike A. Rumore
# All rights reserved.
# Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
#
# ROOT = /home/ombt/ombt

ifndef ROOT
ROOT = $(PWD)
endif

include $(ROOT)/build/makefile.common

SUBDIRS = \
	shsrc \
	libsrc \
	src

TESTSUBDIRS = \
	testsrc 

include $(ROOT)/build/makefile.all
