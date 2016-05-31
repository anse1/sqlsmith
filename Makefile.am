-include Makefile.local

BUILT_SOURCES = gitrev.h
CLEANFILES = gitrev.h

bin_PROGRAMS = sqlsmith
sqlsmith_SOURCES = relmodel.cc schema.cc random.cc expr.cc grammar.cc log.cc dump.cc sqlsmith.cc
sqlsmith_LDADD = $(LIBPQXX_LIBS) $(BOOST_REGEX_LIB)

AM_LDFLAGS = $(BOOST_LDFLAGS)
AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(PQXX_CFLAGS)  -Wall

EXTRA_DIST = gitrev.h dump.hh expr.hh grammar.hh log.hh prod.hh random.hh relmodel.hh schema.hh filter.sql log.sql README.org TODO.org ast.png dump.xsl

gitrev.h: $(HEADERS) $(SOURCES)
	echo "#define GITREV \"$$(git describe --dirty --tags --always)\"" > $@

