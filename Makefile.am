-include Makefile.local

BUILT_SOURCES = gitrev.h
CLEANFILES = gitrev.h

bin_PROGRAMS = sqlsmith
sqlsmith_SOURCES = relmodel.cc schema.cc random.cc expr.cc grammar.cc log.cc dump.cc sqlsmith.cc
sqlsmith_LDADD = $(LIBPQXX_LIBS) $(BOOST_REGEX_LIB)

AM_LDFLAGS = -Wl,--as-needed $(BOOST_LDFLAGS)
AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(PQXX_CFLAGS)  -Wall

EXTRA_DIST = gitrev.h *.hh *.sql *.xsl README.org

gitrev.h: $(HEADERS) $(SOURCES)
	echo "#define GITREV \"$$(git describe --dirty --tags --always)\"" > $@

