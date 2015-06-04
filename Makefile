all: sqlsmith

OBJECTS = relmodel.o schema.o random.o grammar.o sqlsmith.o

CXXFLAGS = -O3 -g

config.h: .git/*
	echo -n '#define GITREV "' > $@
	echo -n "$$(git describe --dirty --tags --always)" >> $@
	echo '"' >> $@

sqlsmith: $(OBJECTS)
	$(CXX) -O3 -g -std=c++11 -Wall $+ -lpqxx -lpq -lstdc++ -o $@

%.o: %.cc config.h *.hh
	$(CXX) $(CXXFLAGS) -std=c++11 -Wall -c $< -o $@

clean:
	rm -f sqlsmith *.o config.h
