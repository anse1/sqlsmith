
sqlsmith: sqlsmith.cc random.cc relmodel.cc grammar.cc
	g++ -std=c++11 -Wall $+ -lpqxx -lpq -o sqlsmith

clean:
	rm -f sqlsmith
