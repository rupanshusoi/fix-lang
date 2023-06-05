extern "C" {
#include "fixpoint_util.h"
}

#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <ctype.h>
#include <string>

#define getrot(TIDX, IDX) get_ro_table_##TIDX(IDX)
#define geti32rom(MIDX) get_i32_ro_mem_##MIDX(0)
#define getrotarg(TIDX, IDX) get_ro_table_##TIDX(IDX + 5)

#define set(TIDX, IDX, VAL) set_rw_table_##TIDX(IDX, VAL)
#define setarg(TIDX, IDX, VAL) set_rw_table_##TIDX(IDX + 5, VAL)

#define atbrom(MIDX, VAL) attach_blob_ro_mem_##MIDX(VAL)
#define attrot(TIDX, VAL) attach_tree_ro_table_##TIDX(VAL)

#define grow(TIDX, SIZE, VAL) grow_rw_table_##TIDX(SIZE, VAL)

#define treerw(TIDX, SIZE) create_tree_rw_table_##TIDX(SIZE)
#define thunk(VAL) create_thunk(VAL)

#define i32(VAL) create_blob_i32(VAL)

#define DEBUG {\
  string line = "dbg: " + to_string(__LINE__); \
  fio(line.c_str(), line.size()); \
}

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

enum Type {TREE, THUNK, BLOB};

void fio(const char *txt, size_t size)
{
  fixpoint_unsafe_io(txt, size);
}

void fio(int x)
{
  string tmp = "int: " + to_string(x);
  fio(tmp.c_str(), tmp.size());
}

