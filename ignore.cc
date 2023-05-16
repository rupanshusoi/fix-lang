#include <stdlib.h>
#include <stdint.h>

extern "C" {
int __cxa_allocate_exception(int x) { abort(); }
void __cxa_throw(int x, int y, int z) { abort(); }
void _start()__attribute__(( export_name("_start" )) ) { return; }
}

