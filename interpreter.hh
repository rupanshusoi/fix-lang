extern "C" {
#include "fixpoint_util.h"
}

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <cassert>

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void ro_mem_1_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "ro_mem_1_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "program_memory_to_rw_0" ) ) );

extern void program_memory_to_rw_1( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "program_memory_to_rw_1" ) ) );

using namespace std;

enum Op {BEGIN, EVAL, APPLY_ADD, APPLY_SUB, APPLY_MUL};

struct Node
{
  bool is_list;
  string arg;
  vector<Node> list;

  Node() = default;
  Node(vector<Node> list) : is_list(true), arg(""), list(list) {};
  Node (string arg) : is_list(false), arg(arg), list(vector<Node>()) {};

  Node& operator[](size_t idx)
  {
    assert(is_list && arg == "");
    return list[idx];
  }

  template<class Archive>
  void serialize(Archive& archive)
  {
    archive(is_list, arg, list);
  }
};

void fio(const char *txt, size_t size)
{
  fixpoint_unsafe_io(txt, size);
}

