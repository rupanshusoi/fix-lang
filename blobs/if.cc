extern "C" {
#include "../fixpoint_util.h"
}

#include "../macros.hh"

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attach_tree_ro_table_0(encode);
  attach_blob_ro_mem_0(get_ro_table_0(2));
  int predicate = get_i32_ro_mem_0(0);
  if (predicate)
  {
    return get_ro_table_0(3); 
  }
  return get_ro_table_0(4);
}
