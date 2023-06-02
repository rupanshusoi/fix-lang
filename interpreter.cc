#include "interpreter.hh"

externref eval()
{
  grow(1, 5, gtro(0, 0));
  strw(1, 0, gtro(0, 0));
  strw(1, 1, gtro(0, 1));
  strw(1, 2, thunk(gtro(0, 4)));
  strw(1, 3, thunk(gtro(0, 5)));
  strw(1, 4, thunk(gtro(0, 6)));
  return thunk(treerw(1, 5));
}

externref apply(Op op)
{
  atbrom(0, gtro(0, 3));
  int x = gti32ro(0);
  atbrom(0, gtro(0, 4));
  int y = gti32ro(0);

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

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attrot(0, encode);
  atbrom(0, gtro(0, 2));
  Op op = static_cast<Op>(gti32ro(0));

  assert(op != BEGIN);

  if (op != EVAL) return apply(op);

  atbrom(0, gtro(0, 3));
  int is_list = gti32ro(0);

  if (is_list) return eval();

  atbrom(0, gtro(0, 4));
  size_t size = byte_size_ro_mem_0();
  char *buf = (char *)malloc(size + 1);
  buf[size] = 0;
  ro_mem_0_to_program_memory((int32_t)buf, 0, size);
  
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
    return gtro(0, 5);
  }
  return i32(strtol(buf, nullptr, 10));
}
