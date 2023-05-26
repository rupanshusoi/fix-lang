extern "C" {
#include "fixpoint_util.h"
}

#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <ctype.h>

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

