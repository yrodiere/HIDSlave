# Process this file with autoconf to produce a configure script.
AC_INIT(src/kb.c)
AM_INIT_AUTOMAKE(xkbd, 0.8.15)
AM_CONFIG_HEADER(config.h)

XFT_LIBS=""
XPM_LIBS=""

dnl check for xft support
AC_ARG_ENABLE(xft,
  [  --disable-xft         disable xft (anti-aliased) support, [default=no]],
  enable_xft=$enableval, enable_xft=yes)

if test x$enable_xft != xno; then
  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)	
  if test x$PKG_CONFIG != xno && $PKG_CONFIG --exists xft; then
      CFLAGS="$CFLAGS -DUSE_XFT `pkg-config --cflags xft`"
      XFT_LIBS="`pkg-config --libs xft`"
  else
    AC_MSG_WARN([*** Could not find pkg-config entry for xft in your path, build may fail***])
    AC_PATH_PROG(XFT_CONFIG, xft-config, no)
      if test x$XFT_CONFIG = xno; then
        AC_MSG_WARN([***Could not find xft-config, build may fail. You may want --disable-xft ***])
        CFLAGS="$CFLAGS -DUSE_XFT"
        XFT_LIBS=" -lXft"
      else
        CFLAGS="$CFLAGS -DUSE_XFT `xft-config --cflags`"
        XFT_LIBS="`xft-config --libs`"
     fi
  fi
fi

AC_ARG_ENABLE(xpm,
  [  --disable-xpm         disable xpm image support, [default=no]],
  enable_xpm=$enableval, enable_xpm=yes)

if test x$enable_xpm != xno; then
      CFLAGS="$CFLAGS -DUSE_XPM"
      XPM_LIBS=" -lXpm"
fi

dnl check for debug build
AC_ARG_ENABLE(debug,
  [  --enable-debug         enable debug build],
  [if test x$enable_debug = xyes; then
      CFLAGS="$CFLAGS -g -DDEBUG"
  fi])


# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_PATH_XTRA
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([strchr strdup])

AC_SUBST(XFT_LIBS)
AC_SUBST(XPM_LIBS)

AC_OUTPUT(Makefile 
          src/Makefile
          data/Makefile
          doc/Makefile 
          data/img/Makefile
          packaging/Makefile
          packaging/ipkg/Makefile
          packaging/ipkg/control )