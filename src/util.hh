#ifndef INTERPRETER_HH

extern "C" {
#include "../includes/fixpoint_util.h"
}

#endif

#include <array>
#include <initializer_list>
#include <cassert>

#define PRELUDE 5

constexpr auto getrot(int tidx, int idx) {
  constexpr std::array fs = {get_ro_table_0, get_ro_table_1, get_ro_table_2,
                             get_ro_table_3};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  return (fs[tidx])(idx);
}

constexpr auto geti32rom(int midx) {
  constexpr std::array fs = {get_i32_ro_mem_0, get_i32_ro_mem_1,
                             get_i32_ro_mem_2, get_i32_ro_mem_3};
  assert(midx >= 0);
  assert(midx <= fs.size());
  return (fs[midx])(0);
}

constexpr auto geti32romz() {
  return geti32rom(0);
}

constexpr auto getrotarg(int tidx, int idx) {
  constexpr std::array fs = {get_ro_table_0, get_ro_table_1, get_ro_table_2,
                             get_ro_table_3};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  return (fs[tidx])(idx + PRELUDE);
}

constexpr auto set(int tidx, int idx, externref ref) {
  constexpr std::array fs = {set_rw_table_0, set_rw_table_1, set_rw_table_2};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  (fs[tidx])(idx, ref);
}

constexpr auto setarg(int tidx, int idx, externref ref) {
  constexpr std::array fs = {set_rw_table_0, set_rw_table_1, set_rw_table_2};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  (fs[tidx])(idx + PRELUDE, ref);
}

constexpr auto atbrom(int midx, externref ref) {
  constexpr std::array fs = {attach_blob_ro_mem_0, attach_blob_ro_mem_1,
                             attach_blob_ro_mem_2, attach_blob_ro_mem_3};
  assert(midx >= 0);
  assert(midx <= fs.size());
  (fs[midx])(ref);
}

constexpr auto atbromz(externref ref) {
  attach_blob_ro_mem_0(ref);
}

constexpr auto attrot(int tidx, externref ref) {
  constexpr std::array fs = {attach_tree_ro_table_0, attach_tree_ro_table_1,
                             attach_tree_ro_table_2, attach_tree_ro_table_3};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  (fs[tidx])(ref);
}

constexpr auto grow(int tidx, int size, externref ref) {
  constexpr std::array fs = {grow_rw_table_0, grow_rw_table_1, grow_rw_table_2};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  (fs[tidx])(size, ref);
}

constexpr auto treerw(int tidx, int size) {
  constexpr std::array fs = {create_tree_rw_table_0, create_tree_rw_table_1,
                             create_tree_rw_table_2};
  assert(tidx >= 0);
  assert(tidx <= fs.size());
  return (fs[tidx])(size);
}

constexpr auto thunk(externref val) { return create_thunk(val); }

constexpr auto i32(int i) { return create_blob_i32(i); }
