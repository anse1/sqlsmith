/// @file
/// @brief grammar: Top-level and unsorted grammar productions

#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <ostream>
#include "relmodel.hh"
#include <memory>
#include "schema.hh"

#include "prod.hh"
#include "expr.hh"

using std::shared_ptr;

struct table_ref : prod {
  vector<shared_ptr<named_relation> > refs;
  static shared_ptr<table_ref> factory(prod *p);
  table_ref(prod *p) : prod(p) { }
  virtual ~table_ref() { }
};

struct table_or_query_name : table_ref {
  void out(std::ostream &out) override;
  table_or_query_name(prod *p);
  ~table_or_query_name() override { }
  named_relation *t;
};

struct target_table : table_ref {
  void out(std::ostream &out) override;
  target_table(prod *p, table *victim = 0);
  ~target_table() override { }
  table *victim_;
};

struct table_sample : table_ref {
  void out(std::ostream &out) override;
  table_sample(prod *p);
  ~table_sample() override { }
  struct table *t;
private:
  string method;
  double percent;
};

struct table_subquery : table_ref {
  bool is_lateral;
  void out(std::ostream &out) override;
  shared_ptr<struct query_spec> query;
  table_subquery(prod *p, bool lateral = false);
  ~table_subquery() override;
  void accept(prod_visitor *v) override;
};

struct lateral_subquery : table_subquery {
  lateral_subquery(prod *p)
    : table_subquery(p, true) {  }
};

struct join_cond : prod {
     static shared_ptr<join_cond> factory(prod *p, table_ref &lhs, table_ref &rhs);
     join_cond(prod *p, table_ref &lhs, table_ref &rhs)
	  : prod(p) { (void) lhs; (void) rhs;}
};

struct simple_join_cond : join_cond {
     std::string condition;
     simple_join_cond(prod *p, table_ref &lhs, table_ref &rhs);
     void out(std::ostream &out) override;
};

struct expr_join_cond : join_cond {
     struct scope joinscope;
     shared_ptr<bool_expr> search;
     expr_join_cond(prod *p, table_ref &lhs, table_ref &rhs);
     void out(std::ostream &out) override;
     void accept(prod_visitor *v) override {
	  search->accept(v);
	  v->visit(this);
     }
};

struct joined_table : table_ref {
  void out(std::ostream &out) override;
  joined_table(prod *p);
  std::string type;
  std::string alias;
  virtual std::string ident() { return alias; }
  shared_ptr<table_ref> lhs;
  shared_ptr<table_ref> rhs;
  shared_ptr<join_cond> condition;
  ~joined_table() override {
  }
  void accept(prod_visitor *v) override {
    lhs->accept(v);
    rhs->accept(v);
    condition->accept(v);
    v->visit(this);
  }
};

struct from_clause : prod {
  std::vector<shared_ptr<table_ref> > reflist;
  void out(std::ostream &out) override;
  from_clause(prod *p);
  ~from_clause() = default;
  void accept(prod_visitor *v) override {
    v->visit(this);
    for (auto p : reflist)
      p->accept(v);
  }
};

struct select_list : prod {
  std::vector<shared_ptr<value_expr> > value_exprs;
  relation derived_table;
  int columns = 0;
  select_list(prod *p);
  void out(std::ostream &out) override;
  ~select_list() { }
  void accept(prod_visitor *v) override {
    v->visit(this);
    for (auto p : value_exprs)
      p->accept(v);
  }
};

struct query_spec : prod {
  std::string set_quantifier;
  shared_ptr<struct from_clause> from_clause;
  shared_ptr<struct select_list> select_list;
  shared_ptr<bool_expr> search;
  std::string limit_clause;
  struct scope myscope;
  void out(std::ostream &out) override;
  query_spec(prod *p, struct scope *s, bool lateral = 0);
  void accept(prod_visitor *v) override {
    v->visit(this);
    select_list->accept(v);
    from_clause->accept(v);
    search->accept(v);
  }
};

struct select_for_update : query_spec {
  const char *lockmode;
  void out(std::ostream &out) override;
  select_for_update(prod *p, struct scope *s, bool lateral = 0);
};

struct prepare_stmt : prod {
  query_spec q;
  static long seq;
  long id;
  void out(std::ostream &out) override {
    out << "prepare prep" << id << " as " << q;
  }
  prepare_stmt(prod *p) : prod(p), q(p, scope) {
    id = seq++;
  }
  void accept(prod_visitor *v) override {
    v->visit(this);
    q.accept(v);
  }
};

struct modifying_stmt : prod {
  table *victim;
  struct scope myscope;
  modifying_stmt(prod *p, struct scope *s, struct table *victim = 0);
//   shared_ptr<modifying_stmt> modifying_stmt::factory(prod *p, struct scope *s);
  virtual void pick_victim();
};

struct delete_stmt : modifying_stmt {
  shared_ptr<bool_expr> search;
  delete_stmt(prod *p, struct scope *s, table *v);
  virtual ~delete_stmt() { }
  void out(std::ostream &out) override {
    out << "delete from " << victim->ident();
    indent(out);
    out << "where " << std::endl << *search;
  }
  void accept(prod_visitor *v) override {
    v->visit(this);
    search->accept(v);
  }
};

struct delete_returning : delete_stmt {
  shared_ptr<struct select_list> select_list;
  delete_returning(prod *p, struct scope *s, table *victim = 0);
  void out(std::ostream &out) override {
    delete_stmt::out(out);
    out << std::endl << "returning " << *select_list;
  }
  void accept(prod_visitor *v) override {
    v->visit(this);
    search->accept(v);
    select_list->accept(v);
  }
};

struct insert_stmt : modifying_stmt {
  vector<shared_ptr<value_expr> > value_exprs;
  insert_stmt(prod *p, struct scope *s, table *victim = 0);
  virtual ~insert_stmt() {  }
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override {
    v->visit(this);
    for (auto p : value_exprs) p->accept(v);
  }
};

struct set_list : prod {
  vector<shared_ptr<value_expr> > value_exprs;
  vector<string> names;
  set_list(prod *p, table *target);
  virtual ~set_list() {  }
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override {
    v->visit(this);
    for (auto p : value_exprs) p->accept(v);
  }
};

struct upsert_stmt : insert_stmt {
  shared_ptr<struct set_list> set_list;
  string constraint;
  shared_ptr<bool_expr> search;
  upsert_stmt(prod *p, struct scope *s, table *v = 0);
  void out(std::ostream &out) override {
    insert_stmt::out(out);
    out << " on conflict on constraint " << constraint << " do update ";
    out << *set_list << " where " << *search;
  }
  void accept(prod_visitor *v) override {
    insert_stmt::accept(v);
    set_list->accept(v);
    search->accept(v);
  }
  virtual ~upsert_stmt() {  }
};

struct update_stmt : modifying_stmt {
  shared_ptr<bool_expr> search;
  shared_ptr<struct set_list> set_list;
  update_stmt(prod *p, struct scope *s, table *victim = 0);
  virtual ~update_stmt() = default;
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override {
    v->visit(this);
    search->accept(v);
  }
};

struct when_clause : prod {
  bool matched;
  shared_ptr<bool_expr> condition;  
//   shared_ptr<prod> merge_action;
  when_clause(struct merge_stmt *p);
  virtual ~when_clause() { }
  static shared_ptr<when_clause> factory(struct merge_stmt *p);
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override;
};

struct when_clause_update : when_clause {
  shared_ptr<struct set_list> set_list;
  struct scope myscope;
  when_clause_update(struct merge_stmt *p);
  ~when_clause_update() override { }
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override;
};

struct when_clause_insert : when_clause {
  vector<shared_ptr<value_expr> > exprs;
  when_clause_insert(struct merge_stmt *p);
  ~when_clause_insert() override { }
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v);
};

struct merge_stmt : modifying_stmt {
  merge_stmt(prod *p, struct scope *s, table *victim = 0);
  shared_ptr<table_ref> target_table_;
  shared_ptr<table_ref> data_source;
  shared_ptr<join_cond> join_condition;
  vector<shared_ptr<when_clause> > clauselist;
  virtual ~merge_stmt() {  }
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override;
};

struct update_returning : update_stmt {
  shared_ptr<struct select_list> select_list;
  update_returning(prod *p, struct scope *s, table *victim = 0);
  void out(std::ostream &out) override {
    update_stmt::out(out);
    out << std::endl << "returning " << *select_list;
  }
  void accept(prod_visitor *v) override {
    v->visit(this);
    search->accept(v);
    set_list->accept(v);
    select_list->accept(v);
  }
};

shared_ptr<prod> statement_factory(struct scope *s);

struct common_table_expression : prod {
  vector<shared_ptr<prod> > with_queries;
  shared_ptr<prod> query;
  vector<shared_ptr<named_relation> > refs;
  struct scope myscope;
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override;
  common_table_expression(prod *parent, struct scope *s);
};

#endif
