dnl Process this file with autoconf to produce a configure script.

AC_INIT(decurs, 0.55.1, [psavo@iki.fi], [decurs])

AC_CONFIG_SRCDIR([src/decurs.cc])
AC_CANONICAL_BUILD()
AC_CANONICAL_HOST()
AC_CANONICAL_TARGET()

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([gnu dist-bzip2])

AC_PROG_CC()
AC_PROG_CXX()


dnl AC_CHECK_HEADERS([sys/types.h sys/stat.h])

PKG_CHECK_MODULES([GTKMM], [glib-2.0 >= 2.0.7 gtk+-2.0 >= 2.0.6 sigc++-1.2 >= 1.2.1 gtkmm-2.0 >= 2.0.0])
AC_SUBST([GTKMM_CFLAGS])
AC_SUBST([GTKMM_LIBS])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
      
AC_OUTPUT()

