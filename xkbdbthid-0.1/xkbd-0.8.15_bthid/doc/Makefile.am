CLEANFILES = xkbd.1
MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = xkbd.1.in style.css docbook.dsl
man_MANS = xkbd.1 
SGML_FILES = manual.sgml

distclean-local:
	rm -f *\~

xkbd.1: xkbd.1.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," xkbd.1.in > xkbd.1

manual.html: $(SGML_FILES)
	-db2html -d `pwd`/docbook.dsl#html manual.sgml 2>/dev/null
	cp style.css manual/

manual.txt: manual.html
	-db2html -u manual.sgml -o manual.html
	w3m -T text/html -dump manual.html > manual.txt
