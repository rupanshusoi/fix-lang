#include "../util.hh"

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  constexpr int tz = 0;
  constexpr int mz = 0;

  attrot(tz, encode);
  atbrom(mz, getrot(tz, 2));

  int predicate = geti32rom(mz);

  if (predicate)
  {
    return getrot(0, 3); 
  }
  return getrot(0, 4);
}
