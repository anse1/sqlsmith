/// @file
/// @brief schema and dut classes for MonetDB


#ifndef MONETDB_HH
#define MONETDB_HH

#include "dut.hh"
#include "relmodel.hh"
#include "schema.hh"
#include <string.h>

#include <mapi.h>

struct monetdb_connection {
	Mapi dbh;
	monetdb_connection(std::string &conninfo);
	void q(const char* query);
	~monetdb_connection();
};

struct schema_monetdb : schema, monetdb_connection {
	schema_monetdb(std::string &conninfo);
	virtual std::string quote_name(const std::string &id) {
		return id;
	}
};

struct dut_monetdb : dut_base, monetdb_connection {
	virtual void test(const std::string &stmt);
	dut_monetdb(std::string &conninfo);
};

#endif
