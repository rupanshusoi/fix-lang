#include <stdlib.h>
#include <stdint.h>

extern "C" {
int __cxa_allocate_exception(int x) { abort(); }
void __cxa_throw(int x, int y, int z) { abort(); }

struct
{
int id;
} pthread_mutex_t;

int pthread_mutex_lock(struct pthread_mutex_t*) { return 0; }
int pthread_mutex_unlock(struct pthread_mutex_t*) { return 0; }
int pthread_cond_broadcast(struct pthread_mutex_t*) { return 0; }
int pthread_cond_wait(struct pthread_mutex_t*) { return 0; }
}

