#ifndef LOG_HH
#define LOG_HH

#include <exception>
#include <pqxx/pqxx>
#include <string>

#include "prod.hh"

struct logger {
  virtual void generated(prod &query) { }
  virtual void executed(prod &query) { }
  virtual void error(prod &query, const pqxx::sql_error &e) { }
};

struct stats_collecting_logger : logger {
  long queries = 0;
  float sum_nodes = 0;
  float sum_height = 0;
  virtual void generated(prod &query);
};

struct cerr_logger : stats_collecting_logger {
  const int columns = 80;
  std::map<std::string, long> errors;
  virtual void generated(prod &query);
  virtual void executed(prod &query);
  virtual void error(prod &query, const pqxx::sql_error &e);
  void report(prod &p);
};

struct pqxx_logger : stats_collecting_logger {
  long id;
  std::shared_ptr<pqxx::connection> c;
  pqxx_logger(std::string target, std::string conninfo);
  virtual void generated(prod &query);
  virtual void error(prod &query, const pqxx::sql_error &e);
};



#endif
