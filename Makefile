all: sqlsmith

SOURCES = $(wildcard *.cc)
OBJECTS = $(patsubst %.cc,%.o,$(SOURCES))

config.h: .git/*
	echo -n '#define GITREV "' > $@
	echo -n "$$(git describe --dirty --tags --always)" >> $@
	echo '"' >> $@

sqlsmith: $(OBJECTS)
	$(CXX) -O3 -g -std=c++11 -Wall $+ -lpqxx -lpq -lstdc++ -o $@

%.o: %.cc config.h
	$(CXX) -O3 -g -std=c++11 -Wall -c $< -o $@

grammar.o: grammar.cc *.hh
relmodel.o: relmodel.cc *.hh
random.o: random.cc *.hh
sqlsmith.o: sqlsmith.cc *.hh
schema.o: schema.cc *.hh

clean:
	rm -f sqlsmith *.o config.h
