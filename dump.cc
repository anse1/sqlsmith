#include <string>
#include <sstream>

#include "dump.hh"
#include "util.hh"

using namespace std;

std::string graphml_dumper::id(struct prod *p)
{
  ostringstream os;
  os << pretty_type(p) << "_" << p;
  return os.str();
}

graphml_dumper::graphml_dumper(ostream &out)
    : o(out)
{
  o << R"(<?xml version="1.0" encoding="UTF-8"?>)" << endl <<
    R"(<graphml xmlns="http://graphml.graphdrawing.org/xmlns" )" <<
    R"(xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" )" <<
    R"(xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns )" <<
    R"(http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">)" << endl;

  o << R"(<key id="retries" for="node" )"
       R"(attr.name="retries" attr.type="double" />)" << endl;
  o << R"(<key id="label" for="node" )"
       R"(attr.name="label" attr.type="string" />)" << endl;
  o << R"(<key id="scope" for="node" )"
       R"(attr.name="scope" attr.type="string" />)" << endl;

  o << R"(<graph id="ast" edgedefault="directed">)" << endl;

}

void graphml_dumper::visit(struct prod *p)
{
  o << R"(<node id=")" << id(p) << R"(">)";
  o << R"(<data key="retries">)" << p->retries << "</data>";
  o << R"(<data key="label">)" << pretty_type(p) << "</data>";
  o << R"(<data key="scope">)" << p->scope << "</data>";
  o << "</node>" << endl;
  if (p->pprod) {
    o << R"(<edge source=")" << id(p) << R"(" target=")" << id(p->pprod) << R"("/>)";
  }
  o << endl;
}

graphml_dumper::~graphml_dumper()
{
  o << "</graph></graphml>" << endl;
}

void ast_logger::generated(prod &query)
{
  string filename{};
  filename += "sqlsmith-";
  filename += to_string(queries);
  filename += ".xml";
  ofstream os(filename);
  graphml_dumper visitor(os);
  query.accept(&visitor);
  queries++;
}
