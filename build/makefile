# Copyright (C) 2016, OMBT LLC and Mike A. Rumore
# All rights reserved.
# Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)

ifndef ROOT
ROOT = $(PWD)/..
endif
ifndef SHELLSCRIPTS
$(error SHELLSCRIPTS is not set)
endif

% : %.sh
	cp $< $@

BINDIR = $(ROOT)/bin

all:	install

install:	$(SHELLSCRIPTS)
		-test -d $(BINDIR) || mkdir -p $(BINDIR);
		-$(foreach script, $(SHELLSCRIPTS), cp $(script).sh $(BINDIR)/$(script); )
		-$(foreach script, $(SHELLSCRIPTS), chmod 755 $(BINDIR)/$(script); )
		touch install

clean:
		-rm install
		-$(foreach script, $(SHELLSCRIPTS), rm -f $(script); )
		-$(foreach script, $(SHELLSCRIPTS), rm -f $(BINDIR)/$(script); )

backup:
		bkup

backupd:
		bkup -d
