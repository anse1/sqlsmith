
sqlsmith: sqlsmith.cc random.cc relmodel.cc schema.cc grammar.cc 
	g++ -g -std=c++11 -Wall $+ -lpqxx -lpq -o sqlsmith

clean:
	rm -f sqlsmith
