-include Makefile.local

BUILT_SOURCES = gitrev.h
bin_PROGRAMS = sqlsmith
sqlsmith_SOURCES = relmodel.cc schema.cc random.cc expr.cc grammar.cc log.cc dump.cc sqlsmith.cc
sqlsmith_LDFLAGS = $(LIBPQXX_LIBS)
sqlsmith_CFLAGS = $(PQXX_CFLAGS)

gitrev.h: $(HEADERS) $(SOURCES) .git/*
	echo "#define GITREV \"$$(git describe --dirty --tags --always)\"" > $@

