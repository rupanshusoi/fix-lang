#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <cassert>
#include <cereal/archives/xml.hpp>
#include <cereal/types/map.hpp>

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "program_memory_to_rw_0" ) ) );

using namespace std;

struct Node
{
  bool is_list;
  string arg;
  vector<Node> list;

  Node(vector<Node> list) : is_list(true), arg(""), list(list) {};
  Node (string arg) : is_list(false), arg(arg), list(vector<Node>()) {};

  Node& operator[](size_t idx)
  {
    assert(is_list && arg == "");
    return list[idx];
  }
};

enum Type {int_t};

struct Value
{
  Type ty;
  int Int;

  Value() = default;
  Value(int i) : ty(int_t), Int(i) {};

  int get_int()
  {
    assert(ty == int_t);
    return Int;
  }
};

struct EvalCtx
{
  Node ast;
  const map<string, int> env;

  EvalCtx(Node ast, const map<string, int> env) : ast(ast), env(env) {}

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(ast, env);
  }
};

struct ApplyCtx
{
  string proc;
  vector<Value> values;

  ApplyCtx(string proc, vector<Value> values) : proc(proc), values(values) {}

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(proc, values);
  }
};

struct Ctx
{
  bool is_eval;
  EvalCtx eval_ctx;
  ApplyCtx apply_ctx;

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(is_eval, eval_ctx, apply_ctx);
  }
};