#include "interpreter.hh"

void bind()
{
  fio("begin bind");
  // Lookup uses a tree consisting of exactly two sub-trees:
  //   The 0th sub-tree has ids
  //   The 1st sub-tree has corresponding values

  attrot(2, getrotarg(0, 0));

  // Get the size of the old env
  attrot(3, getrot(2, ENV));
  attrot(3, getrot(3, 0));
  int old_env_size = size_ro_table_3();
  fio("old_env_size: " + to_string(old_env_size));

  // Prepare to read the formal parameters
  attrot(2, getrotarg(2, 1));
  int nargs = size_ro_table_2() - PRELUDE;

  // Allocate space for first sub-tree
  grow(2, nargs + old_env_size, getrot(0, 0));

  // Copy the old env into the first sub-tree
  for (int i = 0; i < old_env_size; i++)
  {
    set(2, i, getrot(3, i));
  }  

  // Get the formal parameters out of the lambda expr and use them to
  // create the first sub-tree
  for (int i = 0; i < nargs; i++)
  {
    attrot(3, getrotarg(2, i));
    set(2, i + old_env_size, getrotarg(3, 0));
  }
  externref first = treerw(2, nargs + old_env_size);

  // Allocate space for the second sub-tree
  grow(2, nargs + old_env_size, getrot(0, 0));

  // Copy the old env into the second sub-tree
  attrot(3, getrotarg(0, 0));
  attrot(3, getrot(3, ENV));
  attrot(3, getrot(3, 1));
  for (int i = 0; i < old_env_size; i++)
  {
    set(2, i, getrot(3, i));
  }

  // Get the value for each formal parameter (since we are applying a lambda)
  int actuals = size_ro_table_0() - PRELUDE - 1;
  fio("actuals: " + to_string(actuals));
  fio("nargs: " + to_string(nargs));
  fassert(actuals == nargs);

  for (int i = 0; i < nargs; i++)
  {
    set(2, i + old_env_size, getrotarg(0, i + 1));
  }
  externref second = treerw(2, nargs + old_env_size);

  // Combine both sub-trees into one tree
  grow(2, 2, getrot(0, 0));
  set(2, 0, first);
  set(2, 1, second);
  
  // ..and stick this tree into the ENV slot
  set(1, 4, treerw(2, 2));
}

externref apply_lambda(externref encode)
{
  fio("begin apply_lambda");
  attrot(2, getrotarg(0, 0));
  attrot(1, getrotarg(2, 2));

  int size = size_ro_table_1();
  grow(1, size, getrot(0, 0));

  set(1, 0, getrot(1, 0));
  set(1, 1, getrot(1, 1));
  set(1, 2, getrot(1, 2));
  set(1, 3, getrot(1, 3));

  bind();

  for (int i = 0; i < size - PRELUDE; i++)
    setarg(1, i, getrotarg(1, i));

  fio("end apply_lambda");
  return thunk(treerw(1, size));
}

externref apply_wasm()
{
  int nargs = size_ro_table_0() - PRELUDE;
  grow(1, nargs + 1, getrot(0, 0));
  set(1, 0, getrot(0, 0));
  for (int i = 0; i < nargs; i++)
    set(1, i + 1, getrotarg(0, i));
  return thunk(treerw(1, nargs + 1));
}

externref apply(externref encode)
{
  fio("begin apply");
  int type = value_type(getrotarg(0, 0));
  if (type == TREE)
  {
    return apply_lambda(encode);
  }
  fio(type);
  fassert(type == BLOB);
  return apply_wasm();
}

externref copy_env(externref env)
{
  if (value_type(env) != TREE)
  {
    fio("don't copy env blobs");
    return env;
  }
  fio("value_type: " + to_string(value_type(env)));
  attrot(3, env);

  attrot(3, getrot(3, 0));
  int size = size_ro_table_3();

  grow(2, size, getrot(0, 0));

  DEBUG
  for (int i = 0; i < size; i++)
  {
    set(2, i, getrot(3, i));
  }
  DEBUG

  externref first = treerw(2, size);

  attrot(3, env);
  DEBUG
  attrot(3, getrot(3, 1));
  DEBUG

  grow(2, size, getrot(0, 0));

  DEBUG
  for (int i = 0; i < size; i++)
  {
    set(2, i, getrot(3, i));
  }

  externref second = treerw(2, size);

  grow(2, 2, getrot(0, 0));

  set(2, 0, first);
  set(2, 1, second);

  return treerw(2, 2);
}

// Copy env to each arg
void eval_helper(int idx)
{
  fio("begin eval_helper " + to_string(idx));
  attrot(2, getrotarg(0, idx));
  int size = size_ro_table_2();

  grow(1, size, getrot(0, 0));
  set(1, 0, getrot(0, 0));
  set(1, 1, getrot(0, 1));
  set(1, 2, i32(1));
  set(1, 3, getrot(2, IS_LIST));
  set(1, ENV, copy_env(getrot(0, ENV)));

  for (int i = 0; i < size - PRELUDE; i++)
    setarg(1, i, getrotarg(2, i));

  setarg(0, idx, thunk(treerw(1, size)));
  // setarg(0, idx, treerw(1, size));
}

externref eval_list()
{
  fio("begin eval_list");
  int size = size_ro_table_0();
  grow(0, size, getrot(0, 0));
  set(0, 0, getrot(0, 0));
  set(0, 1, getrot(0, 1));
  set(0, 2, i32(0));
  set(0, 3, i32(0));

  // Setting this to getrot(0, 4) somehow breaks this code?
  // set(0, 4, i32(999));
  // set(0, 4, getrot(0, 4));
  // set(0, 4, getrot(0, ENV));
  set(0, 4, copy_env(getrot(0, ENV)));
  // fassert(value_type(getrot(0, ENV)) == TREE);


  fio("size: " + to_string(size));
  for (int i = 0; i < size - PRELUDE; i++)
    eval_helper(i);

  // return treerw(0, size);
  return thunk(treerw(0, size));
}

externref lookup(const char* id)
{
  attrot(1, getrot(0, ENV));

  // Iterate the first sub-tree to look for the id
  attrot(2, getrot(1, 0));
  int size = size_ro_table_2();
  int idx = -1;
  for (int i = 0; i < size; i++)
  {
    DEBUG
    fio(value_type(getrot(2, i)));
    atbromz(getrot(2, i));
    DEBUG
    size_t size = byte_size_ro_mem_0();
    char *buf = (char *)malloc(size + 1);
    buf[size] = 0;
    from_ro_mem_0((int)buf, size);

    if (!strcmp(id, buf))
    {
      idx = i;
      break;
    }
  }
  fassert(idx != -1);

  // Return the corresponding entry from the second sub-tree
  attrot(2, getrot(1, 1));
  fio("end lookup: " + string(id));
  return getrot(2, idx);
}

externref eval_single()
{
  fio("begin eval_single");
  atbromz(getrotarg(0, 0));
  size_t size = byte_size_ro_mem_0();
  char *buf = (char *)malloc(size + 1);
  buf[size] = 0;
  from_ro_mem_0((int32_t)buf, size);
  
  if (isalpha(buf[0]))
  {
    fio("lookup: " + string(buf));
    return lookup(buf);
  }
  int tmp = strtol(buf, nullptr, 10);
  fio("make_int: " + to_string(tmp));
  return i32(tmp);
}

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  fio("begin main");
  attrot(0, encode);
  atbromz(getrot(0, IS_EVAL));
  int is_eval = geti32rom(0);
  if (!is_eval) return apply(encode);

  atbromz(getrot(0, IS_LIST));
  int is_list = geti32rom(0);
  if (is_list)
  {
    attrot(1, getrotarg(0, 0));
    int type = value_type(getrotarg(1, 0));
    if (type != BLOB)
      return eval_list();

    fassert(type == BLOB);
    atbromz(getrotarg(1, 0));
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
