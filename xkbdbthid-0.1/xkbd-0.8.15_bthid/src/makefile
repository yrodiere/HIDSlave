
CFLAGS=-DUSE_XFT -I. -I/usr/include/freetype2 -I/usr/X11R6/include -DUSE_XPM -DDEFAULTCONFIG=\"/var/lib/install/usr/share/xkbdbthid/en_US.qwerty.xkbd\"

all: xkbd.o libXkbd.o button.o box.o kb.o hidcd.o sdp.o
	$(CC) -DUSE_XPM -DUSE_XFT xkbd.o libXkbd.o button.o box.o kb.o hidcd.o sdp.o -o xkbdbthid -L/usr/X11R6/lib /usr/lib/libXft.so /usr/lib/libfreetype.so -lz /usr/lib/libXrender.so -lX11 -lfontconfig ../../../bluez-libs-2.23/src/.libs/libbluetooth.a /usr/lib/libXpm.a

clean:
	rm -f *.o xkbdbthid

distclean: clean
