#include "SharedMemoryConvention.hpp"

#include <stdio.h>
#include <unistd.h>

void lockSharedMemoryWithConvention(SharedMemoryConvention* shmem) {
    uint64_t value = 1;
    uint64_t refcount = __sync_fetch_and_add(&shmem->refcount, value); // Get the value, then do the operation: `*writePtr += value`
    while (refcount != 0) {
        puts("Lock: waiting for refcount");
        usleep(5 // <--milliseconds
               *1000); // (Microseconds after conversion)
        refcount = __sync_fetch_and_sub(&shmem->refcount, value);
        refcount = __sync_fetch_and_add(&shmem->refcount, value);
    }
}
void unlockSharedMemoryWithConvention(SharedMemoryConvention* shmem) {
    uint64_t value = 1;
    uint64_t refcount = __sync_sub_and_fetch(&shmem->refcount, value); // Do the operation, then return the value. Operation is `*writePtr -= value`
}
