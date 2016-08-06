AC_INIT(SQLsmith, 1.1, seltenreich@gmx.de, sqlsmith, https://github.com/anse1/sqlsmith/)
AC_LANG(C++)

AM_INIT_AUTOMAKE(-Wall -Werror foreign)
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)

PKG_CHECK_MODULES(LIBPQXX, libpqxx >= 4.0)

AX_BOOST_BASE()
AX_BOOST_REGEX

AC_SUBST(LIBPQXX_CFLAGS)
AC_SUBST(LIBPQXX_LIBS)

AC_SUBST(CONFIG_GIT_REVISION,
	[m4_esyscmd_s([git describe --dirty --tags --always])])

dnl AC_CHECK_LIB(sqlite3, sqlite3_open_v2)
dnl AM_CONDITIONAL([DUT_SQLITE], [test x$ac_cv_lib_sqlite3_sqlite3_open_v2 = xyes])
AM_CONDITIONAL([DUT_SQLITE], [false])

AC_SUBST(REVISION)

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
 Makefile
 Doxyfile
])
AC_OUTPUT
