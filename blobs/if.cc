extern "C" {
#include "../fixpoint_util.h"
}

#include "../macros.hh"

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attrot(0, encode);
  atbrom(0, getrot(0, 2));
  int predicate = geti32rom(0);
  if (predicate)
  {
    return getrot(0, 3); 
  }
  return getrot(0, 4);
}
