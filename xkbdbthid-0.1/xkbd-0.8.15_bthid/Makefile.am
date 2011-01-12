#EXTRA_DIST = autoconfigure.sh
SUBDIRS = src data packaging doc
EXTRA_DIST = xkbd.desktop xkbd.png

desktopdir = $(datadir)/applications/inputmethods
desktop_DATA = xkbd.desktop

pixmapsdir = $(datadir)/pixmaps/
pixmaps_DATA = xkbd.png

ipkg: Makefile
	$(MAKE) -C packaging/ipkg ipkg
