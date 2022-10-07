#pragma once

#include <cstdint>

template <int imgWidth, int imgHeight>
struct SharedMemoryConventionTemplate {
    static constexpr int imgWidth__ = imgWidth;
    static constexpr int imgHeight__ = imgHeight;
    SharedMemoryConventionTemplate() :
        imgWidth_(imgWidth),
        imgHeight_(imgHeight)
    {}

    // union {
    uint64_t frameCounter;
    //     char msg[8];
    // };
    uint64_t refcount;
    int imgWidth_, imgHeight_;
    uint8_t mat[imgWidth*imgHeight*3];
};

using SharedMemoryConvention = SharedMemoryConventionTemplate<640, 480>;

void lockSharedMemoryWithConvention(SharedMemoryConvention* shmem);
void unlockSharedMemoryWithConvention(SharedMemoryConvention* shmem);
