# Copyright (C) 2016, OMBT LLC and Mike A. Rumore
# All rights reserved.
# Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)

ifndef ROOT
$(error ROOT is not set)
endif
ifndef PERLSCRIPTS
$(error PERLSCRIPTS is not set)
endif
ifndef RELEASEBINDIR
$(error RELEASEBINDIR is not set)
endif

% : %.PL
	cp $< $@
	chmod 744 $@

PERLDIR = $(ROOT)/$(RELEASEBINDIR)

all:	install

install: $(PERLSCRIPTS)
		-test -d $(PERLDIR) || mkdir -p $(PERLDIR);
		-$(foreach script, $(PERLSCRIPTS), cp $(script).PL $(PERLDIR)/$(script); )
		-$(foreach script, $(PERLSCRIPTS), chmod 755 $(PERLDIR)/$(script); )
		touch install

clean:
		-rm install
		-$(foreach script, $(PERLSCRIPTS), rm -f $(script); )
		-$(foreach script, $(PERLSCRIPTS), rm -f $(PERLDIR)/$(script); )

depend:

backup:
		bkup

backupd:
		bkup -d
