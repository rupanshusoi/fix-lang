extern "C" {
#include "fixpoint_util.h"
}

#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <ctype.h>

#define gtro(TIDX, IDX) get_ro_table_##TIDX(IDX)
#define gtrw(TIDX, IDX) get_rw_table_##TIDX(IDX)
#define gti32ro(MIDX) get_i32_ro_mem_##MIDX(0)

#define stro(TIDX, IDX, VAL) set_ro_table_##TIDX(IDX, VAL)
#define strw(TIDX, IDX, VAL) set_rw_table_##TIDX(IDX, VAL)

#define atbrom(MIDX, VAL) attach_blob_ro_mem_##MIDX(VAL)
#define attrot(TIDX, VAL) attach_tree_ro_table_##TIDX(VAL)

#define grow(TIDX, SIZE, VAL) grow_rw_table_##TIDX(SIZE, VAL)

#define treerw(TIDX, SIZE) create_tree_rw_table_##TIDX(SIZE)
#define thunk(VAL) create_thunk(VAL)

#define i32(VAL) create_blob_i32(VAL)

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "wasi_snapshot_preview1" ), import_name( "program_memory_to_rw_0" ) ) );

using namespace std;

enum Op {BEGIN, EVAL, APPLY_ADD, APPLY_SUB, APPLY_MUL};

void fio(const char *txt, size_t size)
{
  fixpoint_unsafe_io(txt, size);
}

