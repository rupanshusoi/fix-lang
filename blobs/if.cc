#include "../util.hh"

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  constexpr int tz = 0;

  attrot(tz, encode);
  atbromz(getrot(tz, 2));

  int predicate = geti32romz();

  if (predicate)
  {
    return getrot(0, 3); 
  }
  return getrot(0, 4);
}
