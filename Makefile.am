SUBDIRS = m4 intl po src doc

ACLOCAL_AMFLAGS = -I m4

pkgdata_DATA = README FAQ doc/sample.smbcrc

pdf:	
	cd doc \
	&& make pdf
ps:	
	cd doc \
	&& make ps

dvi:	
	cd doc \
	&& make dvi

html:	
	cd doc \
	&& make html

html-no-split:	
	export MAKEINFOFLAGS=--no-split \
	&& make html

doc-r:	
	cd doc \
        && rm -fR manual \
        && make pdf \
	&& ./gendocs.sh smbc "Samba Commander (Smbc) 1.2.2 Manual - Table of Contents"

clean-r:
	rm -fR doc/manual \
	&& make clean

