#ifndef LOG_HH
#define LOG_HH

#include <exception>
#include <pqxx/pqxx>
#include <chrono>
#include <string>
#include <memory>

#include "prod.hh"

struct logger {
  virtual void generated(prod &query) { }
  virtual void executed(prod &query) { }
  virtual void error(prod &query, const pqxx::sql_error &e) { }
};

struct cerr_logger : logger {
  const int columns = 80;
  int queries = 0;
  float sum_nodes = 0;
  float sum_height = 0;
  std::map<std::string, long> errors;
  virtual void executed(prod &query);
  virtual void error(prod &query, const pqxx::sql_error &e);
  void report(prod &p);
};

struct pqxx_logger : logger {
  std::shared_ptr<pqxx::connection> c;
  pqxx_logger(std::string conninfo);
  virtual void error(prod &query, const pqxx::sql_error &e);
};



#endif
