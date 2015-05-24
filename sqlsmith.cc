#include <iostream>
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

struct schema {
  vector<string> tables;
};

struct schema_pqxx : public schema {
  schema_pqxx(connection c&) {
    work w(c);
    result r = w.exec("select table_name from information_schema.tables;");

    for (result::const_iterator row = r.begin();
	 row != r.end();
	 ++row)
      {
	for (result::tuple::const_iterator field = row->begin();
	     field != row->end();
	     ++field)
	  cout << field->c_str() << '\t';
	cout << endl;
      }
  }
};

struct prod {
  virtual string to_string() = 0;
};

struct query_spec : public prod {
  std::string to_string() {
    return std::string("select version();");
  }
};

int main()
{
  try
    {
      connection c;
      work w(c);
      w.exec("set transaction isolation level serializable;");
      query_spec gen;
      result r = w.exec(gen.to_string());
      w.commit();
      cout << r[0][0].as<string>() << endl;
    }
  catch (const std::exception &e)
    {
      cerr << e.what() << endl;
      return 1;
    }
}
