BIN=$(DESTDIR)/usr/bin/
SHARE=$(DESTDIR)/usr/share/xkbdbthid
IMG=$(DESTDIR)/usr/share/xkbdbthid/img
APPDIR = $(DESTDIR)/usr/share/applications/hildon
ETCDIR = $(DESTDIR)/etc/others-menu/extra_applications

all:
	cd bthid/xkbdbthidwrapper; make
	cd xkbd-0.8.15_bthid; ./configure; cd src; make -f makefile

clean:
	cd bthid/xkbdbthidwrapper; make clean
	cd xkbd-0.8.15_bthid; make distclean

install: all
	install -d $(BIN)
	install -d $(SHARE)
	install -d $(ETCDIR)
	install -d $(APPDIR)
	install -d $(IMG)
	install ./bthid/img/return.xpm $(IMG)
	install ./bthid/img/shift.xpm $(IMG)
	install ./bthid/img/tab.xpm $(IMG)
	install ./bthid/img/backspace.xpm $(IMG)
	install ./bthid/xkbdbthidwrapper/xkbdbthid.sh $(BIN)
	install -m 644 ./bthid/en_US.qwerty.xkbd $(SHARE)
	install ./xkbd-0.8.15_bthid/src/xkbdbthid $(BIN)
	install -o root -m 4555 ./bthid/xkbdbthidwrapper/xkbdbthidwrapper $(BIN)
	install ./osso-xkbdbthid.desktop $(APPDIR)
	install ./osso-xkbdbthid.desktop $(ETCDIR)/0301_osso-xkbdbthid.desktop
