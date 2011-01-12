DEFAULT_CONFIG=$(pkgdatadir)/en_GB.qwerty.xkbd

LDADD = @X_LIBS@ -lX11 -lXtst $(XPM_LIBS) $(XFT_LIBS) -lbluetooth
CFLAGS = @CFLAGS@ -DDEFAULTCONFIG=\"$(DEFAULT_CONFIG)\"

bin_PROGRAMS = xkbd
xkbd_SOURCES = xkbd.c libXkbd.c box.c button.c kb.c \
                      structs.h box.h  button.h  kb.h \
                      libXkbd.h hidcd.c hidcd.h hid.h sdp.h sdp.c
#xkbd_SOURCES = xkbd.c libXkbd.c box.c button.c kb.c libvirtkeys.c \
#                      structs.h box.h  button.h  kb.h  libvirtkeys.h \
#                      libXkbd.h hidcd.c hidcd.h hid.h sdp.h sdp.c
xkbd_LDADD   = $(LDADD)
        

