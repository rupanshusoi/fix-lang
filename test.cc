extern "C" {
#include "../util/fixpoint_util.h"
}

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "program_memory_to_rw_0" ) ) );

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cereal/archives/xml.hpp>

using namespace std;

struct State
{
  int x;

  State(int x) : x(x) {}

  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( x );
  }
};

/* encode[0]: resource limits
   encode[1]: this program
   encode[2]: serialized state
*/
__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  stringstream ss;
  {
    cereal::XMLOutputArchive oarchive(ss);
    State state(2);
    oarchive(state);
  }
  return create_blob_i32(93);
}
