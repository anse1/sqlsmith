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

AC_SEARCH_LIBS(sqlite3_open_v2, sqlite3)

dnl , [ACTION-IF-FOUND],
dnl          [ACTION-IF-NOT-FOUND], [OTHER-LIBRARIES])

AC_SUBST(REVISION)


AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
 Makefile
 Doxyfile
])
AC_OUTPUT
