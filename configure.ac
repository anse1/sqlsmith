AC_INIT(SQLsmith, 1.2.1, seltenreich@gmx.de, sqlsmith, https://github.com/anse1/sqlsmith/)
AC_LANG(C++)

AM_INIT_AUTOMAKE(-Wall -Werror foreign)
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)

AX_LIB_POSTGRESQL()
PKG_CHECK_MODULES(LIBPQXX, libpqxx >= 4.0,
		[AC_DEFINE([HAVE_REDSHIFT], [1], [define if the LibPQ is available])],
		[]
)
AM_CONDITIONAL([DUT_REDSHIFT], [test x$pkg_failed = xno])

PKG_CHECK_MODULES(MONETDB_MAPI, monetdb-mapi >= 11.23.0,
		[AC_DEFINE([HAVE_MONETDB], [1], [define if the MonetDB client library is available])],
		[]
)

AM_CONDITIONAL([DUT_MONETDB], [test x$pkg_failed = xno])

AX_BOOST_BASE()
AX_BOOST_REGEX

AC_SUBST(LIBPQXX_CFLAGS)
AC_SUBST(LIBPQXX_LIBS)

AC_SUBST(CONFIG_GIT_REVISION,
	[m4_esyscmd_s([git describe --dirty --tags --always])])

AC_CHECK_LIB(sqlite3, sqlite3_open_v2)
AM_CONDITIONAL([DUT_SQLITE], [test x$ac_cv_lib_sqlite3_sqlite3_open_v2 = xyes])

AC_SUBST(REVISION)

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
 Makefile
 Doxyfile
])
AC_OUTPUT
