AC_INIT(SQLsmith, 1.4, seltenreich@gmx.de, sqlsmith, https://github.com/anse1/sqlsmith/)
AC_LANG(C++)

AM_INIT_AUTOMAKE(-Wall -Werror foreign)
AC_PROG_CXX

AX_LIB_POSTGRESQL()

PKG_CHECK_MODULES(LIBPQXX, libpqxx < 7.0,
        [AX_CXX_COMPILE_STDCXX_11(noext,mandatory)],
        [PKG_CHECK_MODULES([LIBPQXX], [libpqxx >= 7.0],
                 [libpqxx7=yes],
                 []
         )]
)

AS_IF([test "x$libpqxx7" = "xyes"], AC_MSG_NOTICE([libpqxx version >= 7 detected]), [])
AS_IF([test "x$libpqxx7" = "xyes"], [AC_DEFINE([HAVE_LIBPQXX7], [1], [define if libpqxx >= 7 is used])], [])
AS_IF([test "x$libpqxx7" = "xyes"], [AX_CXX_COMPILE_STDCXX_17(noext,mandatory)], [])

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
