SUBDIRS = img
kbdconfigdir = $(pkgdatadir)
kbdconfig_DATA = en_GB.qwerty.xkbd en_GB.qwerty.tiny.xkbd \
                 en_US.qwerty.xkbd en_all.fitaly.xkbd kbdconfig.numsonly \
                 en_GB.qwerty.tabletpc.xkbd

CLEANFILES = en_GB.qwerty.xkbd en_GB.qwerty.tiny.xkbd \
             en_US.qwerty.xkbd en_all.fitaly.xkbd kbdconfig.numsonly \
	     en_GB.qwerty.tabletpc.xkbd

EXTRA_DIST = en_GB.qwerty.xkbd.in en_GB.qwerty.tiny.xkbd.in \
             en_US.qwerty.xkbd.in en_all.fitaly.xkbd.in kbdconfig.numsonly.in \
             en_GB.qwerty.tabletpc.xkbd.in

# en_GB.qwerty.xkbd
# en_GB.qwerty.tiny.xkbd
# en_US.qwerty.xkbd
# en_all.fitaly.xkbd

en_all.fitaly.xkbd: en_all.fitaly.xkbd.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," \
	en_all.fitaly.xkbd.in >  en_all.fitaly.xkbd 

en_US.qwerty.xkbd: en_US.qwerty.xkbd.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," \
	en_US.qwerty.xkbd.in > en_US.qwerty.xkbd 

en_GB.qwerty.tiny.xkbd: en_GB.qwerty.tiny.xkbd.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," \
	en_GB.qwerty.tiny.xkbd.in > en_GB.qwerty.tiny.xkbd 

en_GB.qwerty.tabletpc.xkbd: en_GB.qwerty.tabletpc.xkbd.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," \
	en_GB.qwerty.tabletpc.xkbd.in > en_GB.qwerty.tabletpc.xkbd 

kbdconfig.numsonly: kbdconfig.numsonly.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," \
	kbdconfig.numsonly.in >  kbdconfig.numsonly 

en_GB.qwerty.xkbd: en_GB.qwerty.xkbd.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," \
	en_GB.qwerty.xkbd.in > en_GB.qwerty.xkbd 

