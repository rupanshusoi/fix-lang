extern "C" {
#include "fixpoint_util.h"
}

#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <ctype.h>
#include <string>

#include "macros.hh"

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "program_memory_to_rw_0" ) ) );

void from_ro_mem_0(int32_t program_offset, int32_t len)
{
  ro_mem_0_to_program_memory(program_offset, 0, len);
}

using namespace std;

enum Op {APPLY_ADD, APPLY_SUB, APPLY_MUL};

enum Idx {LIMITS, PROGRAM, IS_EVAL, IS_LIST, ENV};
#define PRELUDE 5 // Number of elements in Idx

enum Type {TREE, THUNK, BLOB};

void fio(const char *txt)
{
  fixpoint_unsafe_io(txt, strlen(txt));
}

void fio(int x)
{
  string tmp = "int: " + to_string(x);
  fio(tmp.c_str());
}

