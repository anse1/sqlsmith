
sqlsmith: sqlsmith.cc random.cc relmodel.cc grammar.cc
	g++ -std=c++14 -Wall $+ -lpqxx -lpq -o sqlsmith
