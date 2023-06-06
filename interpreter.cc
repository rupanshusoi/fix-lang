#include "interpreter.hh"

externref apply_lambda()
{
  atbrom(0, getrotarg(0, 1));
  int x = geti32rom(0);
  atbrom(0, getrotarg(0, 2));
  int y = geti32rom(0);

  attrot(2, getrotarg(0, 0));
  attrot(1, getrotarg(2, 2));
  int size = size_ro_table_1();
  grow(1, size, getrot(0, 0));
  set(1, 0, getrot(1, 0));
  set(1, 1, getrot(1, 1));
  set(1, 2, getrot(1, 2));
  set(1, 3, getrot(1, 3));

  // Lookup is a tree that has two sub-trees:
  //   First contains the identifiers
  //   Second contains the values

  // First half:
  grow(2, 2, getrot(0, 0));
  string xstr = "x";
  grow_rw_mem_0_pages(2);
  program_memory_to_rw_0(0, (int32_t)xstr.c_str(), xstr.size());
  set(2, 0, create_blob_rw_mem_0(xstr.size()));
  string ystr = "y";
  grow_rw_mem_0_pages(2);
  program_memory_to_rw_0(0, (int32_t)ystr.c_str(), ystr.size());
  set(2, 1, create_blob_rw_mem_0(xstr.size()));

  externref first = treerw(2, 2);

  // Second half:
  grow(2, 2, getrot(0, 0));
  set(2, 0, i32(x)); 
  set(2, 1, i32(y)); 
  externref second = treerw(2, 2);

  // Combine
  grow(2, 2, getrot(0, 0));
  set(2, 0, first);
  set(2, 1, second);
  
  set(1, 4, treerw(2, 2));

  for (int i = 0; i < size - PRELUDE; i++)
    setarg(1, i, getrotarg(1, i));

  // return treerw(1, size);
  return thunk(treerw(1, size));
}

externref apply_primitive()
{
  atbrom(0, getrotarg(0, 0));
  Op op = static_cast<Op>(geti32rom(0));

  atbrom(0, getrotarg(0, 1));
  int x = geti32rom(0);

  atbrom(0, getrotarg(0, 2));
  int y = geti32rom(0);

  switch (op)
  {
    case APPLY_ADD:
      return i32(x + y);
    case APPLY_SUB:
      return i32(x - y);
    case APPLY_MUL:
      return i32(x * y);
    default:
      fassert(false);
  }
}

externref apply()
{
  int type = value_type(getrotarg(0, 0));
  if (type == TREE)
    return apply_lambda();
  return apply_primitive();
}

void eval_helper(int idx)
{
  attrot(2, getrotarg(0, idx));
  int size = size_ro_table_2();

  grow(1, size, getrot(0, 0));
  set(1, 0, getrot(0, 0));
  set(1, 1, getrot(0, 1));
  set(1, 2, i32(1));
  set(1, 3, getrot(2, IS_LIST));
  set(1, ENV, getrot(0, ENV));

  for (int i = 0; i < size - PRELUDE; i++)
    setarg(1, i, getrotarg(2, i));

  setarg(0, idx, thunk(treerw(1, size)));
}

externref eval_list()
{
  int size = size_ro_table_0();
  grow(0, size, getrot(0, 0));
  set(0, 0, getrot(0, 0));
  set(0, 1, getrot(0, 1));
  set(0, 2, i32(0));
  set(0, 3, i32(0));
  set(0, 4, i32(0));

  for (int i = 0; i < size - PRELUDE; i++)
    eval_helper(i);

  return thunk(treerw(0, size));
}

externref lookup(const char* id)
{
  attrot(1, getrot(0, ENV));

  // First iterate through the first subtree to find id
  attrot(2, getrot(1, 0));
  int size = size_ro_table_2();
  int idx = -1;
  for (int i = 0; i < size; i++)
  {
    atbrom(0, getrot(2, i));
    size_t size = byte_size_ro_mem_0();
    char *buf = (char *)malloc(size + 1);
    buf[size] = 0;
    from_ro_mem_0((int32_t)buf, size);

    if (!strcmp(id, buf)) { idx = i; break; }
  }
  fassert(idx != -1);

  // Now get the corresponding entry from the second subtree
  attrot(2, getrot(1, 1));
  return getrot(2, idx);
}

externref eval_single()
{
  atbrom(0, getrotarg(0, 0));
  size_t size = byte_size_ro_mem_0();
  char *buf = (char *)malloc(size + 1);
  buf[size] = 0;
  from_ro_mem_0((int32_t)buf, size);
  
  if (!strcmp(buf, "+"))
  {
    return i32(APPLY_ADD);
  }
  else if (!strcmp(buf, "-"))
  {
    return i32(APPLY_SUB);
  }
  else if (!strcmp(buf, "*"))
  {
    return i32(APPLY_MUL);
  }
  else if (isalpha(buf[0]))
  {
    return lookup(buf);
  }
  return i32(strtol(buf, nullptr, 10));
}

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attrot(0, encode);
  atbrom(0, getrot(0, IS_EVAL));
  int is_eval = geti32rom(0);
  if (!is_eval) return apply();

  atbrom(0, getrot(0, IS_LIST));
  int is_list = geti32rom(0);
  if (is_list)
  {
    attrot(1, getrotarg(0, 0));
    int type = value_type(getrotarg(1, 0));
    if (type != BLOB)
      return eval_list();

    fassert(type == BLOB);
    atbrom(0, getrotarg(1, 0));
    size_t size = byte_size_ro_mem_0();
    char *buf = (char *)malloc(size + 1);
    buf[size] = 0;
    from_ro_mem_0((int32_t)buf, size); 

    // Check for special forms
    if (!strcmp(buf, "lambda"))
    {
      return encode;
    }
    else
      return eval_list();
  }

  return eval_single();
}
