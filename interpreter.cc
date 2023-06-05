#include "interpreter.hh"

// Extend env, call eval
externref apply()
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
      assert(false);
  }
}

void eval_helper(int idx)
{
  grow(1, 6, getrot(0, 0));
  set(1, 0, getrot(0, 0));
  set(1, 1, getrot(0, 1));
  set(1, 2, i32(1));
  set(1, 3, i32(0));
  set(1, ENV, getrot(0, ENV));
  attrot(2, getrotarg(0, idx));
  setarg(1, 0, getrotarg(2, 0));
  setarg(0, idx, thunk(treerw(1, 6)));
}

externref eval_list()
{
  grow(0, 8, getrot(0, 0));
  set(0, 0, getrot(0, 0));
  set(0, 1, getrot(0, 1));
  set(0, 2, i32(0));
  set(0, 3, i32(0));
  set(0, 4, i32(0));

  eval_helper(0);
  eval_helper(1);
  eval_helper(2);

  return thunk(treerw(0, 8));
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
  if (is_list) return eval_list();

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
    return getrot(0, 5);
  }
  return i32(strtol(buf, nullptr, 10));
}
