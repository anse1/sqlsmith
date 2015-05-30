
config.h: .git/*
	echo -n '#define GITREV "' > $@
	echo -n "$$(git describe --dirty --tags --always)" >> $@
	echo '"' >> $@

sqlsmith: *.cc config.h
	g++ -O3 -g -std=c++11 -Wall *.cc -lpqxx -lpq -o sqlsmith

clean:
	rm -f sqlsmith config.h
