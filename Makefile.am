-include Makefile.local

BUILT_SOURCES = gitrev.h

bin_PROGRAMS = sqlsmith

DUT = postgres.cc

if DUT_MONETDB
DUT += monetdb.cc
endif

if DUT_SQLITE
DUT += sqlite.cc
endif


sqlsmith_SOURCES = relmodel.cc schema.cc $(DUT)	 			\
    random.cc prod.cc expr.cc grammar.cc log.cc dump.cc impedance.cc	\
    sqlsmith.cc

sqlsmith_LDADD = $(LIBPQXX_LIBS) $(MONETDB_MAPI_LIBS) $(BOOST_REGEX_LIB) $(POSTGRESQL_LIBS)

AM_LDFLAGS = $(BOOST_LDFLAGS) $(POSTGRESQL_LDFLAGS)
AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(LIBPQXX_CFLAGS) $(POSTGRESQL_CPPFLAGS) $(MONETDB_MAPI_CFLAGS) -Wall -Wextra


EXTRA_DIST = gitrev.h dump.hh expr.hh grammar.hh log.hh prod.hh		\
    random.hh relmodel.hh schema.hh impedance.hh known.txt known_re.txt log.sql	\
    README.org TODO.org ast.png logo.png dump.xsl util.hh sqlite.hh	\
    dut.hh postgres.hh monetdb.hh log-v1.0-to-v1.2.sql

gitrev.h: $(HEADERS) $(SOURCES)
	-if git describe --dirty --tags --always > /dev/null ; then \
	echo "#define GITREV \"$$(git describe --dirty --tags --always)\"" > $@ ;\
	else \
	echo "#define GITREV \"unreleased\"" > $@ ;\
	fi

filterdump:
	psql -Xc 'copy (select error from known) to stdout' |sort -u > known.txt
	psql -Xc 'copy (select re from known_re) to stdout' |sort -u > known_re.txt
	psql -Xc 'copy (select sqlstate from boring_sqlstates) to stdout' |sort -u > boring_sqlstates.txt

.PHONY: filterdump
