CLEANFILES = menu
EXTRA_DIST = control.in postinst postrm menu.in 90xXkbd

#clean-generic:
#        rm -rf nothing yet

menu: menu.in 
	cat menu.in > menu

ipkg: Makefile control menu
	mkdir -p  ./build/CONTROL
	cp control ./build/CONTROL/
	cp postrm ./build/CONTROL/
	cp postinst ./build/CONTROL/
	mkdir -p ./build/etc/X11/Xsession.d/
	cp 90xXkbd ./build/etc/X11/Xsession.d/
	mkdir -p ./build/usr/lib/menu
	cp menu ./build/usr/lib/menu/xkbd; 
	mkdir -p ./build/usr/share/pixmaps
	mkdir -p ./build/usr/share/matchbox/mbdock
	fakeroot make DESTDIR=`pwd`/build install-strip -C ../../
	rm -fr ./build/usr/man
	ipkg-build ./build ./
