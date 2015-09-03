-include Makefile.local

BUILT_SOURCES = gitrev.h
bin_PROGRAMS = sqlsmith
sqlsmith_SOURCES = relmodel.cc schema.cc random.cc expr.cc grammar.cc log.cc dump.cc sqlsmith.cc

AM_LDFLAGS = $(BOOST_LDFLAGS) $(LIBPQXX_LIBS) $(BOOST_REGEX_LIB)
AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(PQXX_CFLAGS)  -Wall

EXTRA_DIST = *.hh *.sql *.xsl README.org

gitrev.h: $(HEADERS) $(SOURCES) .git/*
	echo "#define GITREV \"$$(git describe --dirty --tags --always)\"" > $@

