#define getrot(TIDX, IDX) get_ro_table_##TIDX(IDX)
#define geti32rom(MIDX) get_i32_ro_mem_##MIDX(0)
#define getrotarg(TIDX, IDX) get_ro_table_##TIDX(IDX + PRELUDE)

#define set(TIDX, IDX, VAL) set_rw_table_##TIDX(IDX, VAL)
#define setarg(TIDX, IDX, VAL) set_rw_table_##TIDX(IDX + PRELUDE, VAL)

#define atbrom(MIDX, VAL) attach_blob_ro_mem_##MIDX(VAL)
#define attrot(TIDX, VAL) attach_tree_ro_table_##TIDX(VAL)

#define grow(TIDX, SIZE, VAL) grow_rw_table_##TIDX(SIZE, VAL)

#define treerw(TIDX, SIZE) create_tree_rw_table_##TIDX(SIZE)
#define thunk(VAL) create_thunk(VAL)

#define i32(VAL) create_blob_i32(VAL)

#define DEBUG {\
  string line = "dbg: " + to_string(__LINE__); \
  fio(line.c_str()); \
}

#define fassert(X) {\
  if (!(X)) {\
  string line = "assert failed at line: " + to_string(__LINE__); \
  fio(line.c_str()); \
  assert(false); \
  }\
}
