#include "interpreter.hh"

externref eval()
{
  grow_rw_table_1(5, get_ro_table_0(0));
  set_rw_table_1(0, get_ro_table_0(0));
  set_rw_table_1(1, get_ro_table_0(1));

  attach_blob_ro_mem_0(get_ro_table_0(4));
  size_t size = byte_size_ro_mem_0();
  char *op = (char *)malloc(size);
  ro_mem_0_to_program_memory((int32_t)op, 0, size);
  string op_str(op, size);

  if (op_str == "+")
  {
    set_rw_table_1(2, create_blob_i32(APPLY_ADD));
  }
  else if (op_str == "-")
  {
    set_rw_table_1(2, create_blob_i32(APPLY_SUB));
  }
  else if (op_str == "*")
  {
    set_rw_table_1(2, create_blob_i32(APPLY_MUL));
  }
  else
    assert(false);

  set_rw_table_1(3, get_ro_table_0(5));
  set_rw_table_1(4, get_ro_table_0(6));
  return create_thunk(create_tree_rw_table_1(5));
}

externref apply(Op op)
{
  attach_blob_ro_mem_0(get_ro_table_0(3));
  int x = get_i32_ro_mem_0(0);
  attach_blob_ro_mem_0(get_ro_table_0(4));
  int y = get_i32_ro_mem_0(0);

  switch (op)
  {
    case APPLY_ADD:
      return create_blob_i32(x + y);
    case APPLY_SUB:
      return create_blob_i32(x - y);
    case APPLY_MUL:
      return create_blob_i32(x * y);
    default:
      assert(false);
  }
}

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attach_tree_ro_table_0(encode);
  attach_blob_ro_mem_0(get_ro_table_0(2));
  Op op = static_cast<Op>(get_i32_ro_mem_0(0));

  assert(op != BEGIN);

  if (op == EVAL)
  {
    attach_blob_ro_mem_0(get_ro_table_0(3));
    int is_list = get_i32_ro_mem_0(0);
    if (!is_list)
    {
      attach_blob_ro_mem_0(get_ro_table_0(4));
      size_t size = byte_size_ro_mem_0();
      char *buf = (char *)malloc(size);
      ro_mem_0_to_program_memory((int32_t)buf, 0, size);
      string s(buf, size);
      return create_blob_i32(stoi(s));
    }
    return eval();      
  }
  
  return apply(op);
}
