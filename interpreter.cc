#include "interpreter.hh"

// Extend env, call eval
externref apply_lambda()
{
  atbrom(0, getrotarg(0, 1));
  int val = geti32rom(0);

  attrot(1, getrotarg(0, 0));
  int size = size_ro_table_1();
  fio(size);
  grow(1, size, getrot(0, 0));
  set(1, 0, getrot(1, 0));
  set(1, 1, getrot(1, 1));
  set(1, 2, getrot(1, 2));
  set(1, 3, getrot(1, 3));
  set(1, 4, i32(val));

  for (int i = 0; i < size - 5; i++)
    setarg(1, i, getrotarg(1, i));

  // return treerw(1, size);
  return thunk(treerw(1, size));
}

externref apply()
{
  int type = value_type(getrotarg(0, 0));
  if (type == TREE)
    return apply_lambda();

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
      assert(false);
  }
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

  for (int i = 0; i < size - 5; i++)
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

  for (int i = 0; i < size - 5; i++)
    eval_helper(i);

  return thunk(treerw(0, size));
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

    assert(type == BLOB);
    atbrom(0, getrotarg(1, 0));
    size_t size = byte_size_ro_mem_0();
    char *buf = (char *)malloc(size + 1);
    buf[size] = 0;
    from_ro_mem_0((int32_t)buf, size); 

    if (!strcmp(buf, "lambda"))
    {
      // Return the raw expr
      return getrotarg(0, 2);
    }
    else
      return eval_list();
  }

  // Culprit:
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
    // Fixme
    return getrot(0, ENV);
  }
  return i32(strtol(buf, nullptr, 10));
}
