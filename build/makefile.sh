# Copyright (C) 2016, OMBT LLC and Mike A. Rumore
# All rights reserved.
# Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)

ifndef ROOT
$(error ROOT is not set)
endif
ifndef SHELLSCRIPTS
$(error SHELLSCRIPTS is not set)
endif
ifndef RELEASEBINDIR
$(error RELEASEBINDIR is not set)
endif

% : %.sh
	cp $< $@

SHELLDIR = $(ROOT)/$(RELEASEBINDIR)

all:	install

depend:

install:	$(SHELLSCRIPTS)
		-test -d $(SHELLDIR) || mkdir -p $(SHELLDIR);
		-$(foreach script, $(SHELLSCRIPTS), cp $(script).sh $(SHELLDIR)/$(script); )
		-$(foreach script, $(SHELLSCRIPTS), chmod 755 $(SHELLDIR)/$(script); )
		touch install

clean:
		-rm install
		-$(foreach script, $(SHELLSCRIPTS), rm -f $(script); )
		-$(foreach script, $(SHELLSCRIPTS), rm -f $(SHELLDIR)/$(script); )

backup:
		bkup

backupd:
		bkup -d
