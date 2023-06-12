#include "../src/util.hh"

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  constexpr int tz = 0;

  attrot(tz, encode);

  atbromz(getrot(tz, 2));
  int lhs = geti32romz();

  atbromz(getrot(tz, 3));
  int rhs = geti32romz();

  return i32(lhs == rhs);
}
