#include "sharedMemory.hpp"

// https://github.com/lassik/shm_open_anon from https://stackoverflow.com/questions/55704758/is-there-anything-like-shm-open-without-filename . More reference: https://nullprogram.com/blog/2016/04/10/

extern "C" {
#include "shm_open_anon/shm_open_anon.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <stdint.h>

// https://github.com/lassik/shm_open_anon/blob/master/test_shared.c

void
diesys(const char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	//exit(1);
}

int
fd_without_close_on_exec(int fd)
{
	int flags;

	if ((flags = fcntl(fd, F_GETFD)) == -1) {
		diesys("get close-on-exec");
		return -1;
	}
	flags &= ~FD_CLOEXEC;
	if (fcntl(fd, F_SETFD, flags) == -1) {
		diesys("set close-on-exec");
		return -1;
	}
	return fd;
}

int
shm_open_anon_shared(void)
{
        int fd;

        if ((fd = shm_open_anon()) == -1) {
                diesys("shm_open_anon()");
		return -1;
	}
	return fd_without_close_on_exec(fd);
}

void *
map_shared_memory_from_fd(int fd, size_t *out_size)
{
	struct stat st;
	void *buf;
	size_t size;

	if (fstat(fd, &st) == -1) {
		diesys("fstat");
		return nullptr;
	}
	*out_size = size = (size_t)st.st_size;
	buf = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		diesys("mmap");
		return nullptr;
	}
	return buf;
}

void* create_shared_memory(size_t size, int* out_fd) {
  char *buf;
  size_t bufsize = size;
  int fd;

  if ((fd = shm_open_anon_shared()) == -1) {
    diesys("shm_open_anon_shared");
    return nullptr;
  }
  if (ftruncate(fd, (off_t)bufsize) == -1) {
    diesys("ftruncate");
    return nullptr;
  }
  buf = (char*)map_shared_memory_from_fd(fd, &bufsize);
  memset(buf, 0, bufsize);
  //snprintf(buf, bufsize, "hello from parent");
  
  // Show size for debugging purposes
	struct stat st;
	if (fstat(fd, &st) == -1) {
		diesys("fstat");
		return nullptr;
	}
    printf("create_shared_memory() with fd %d, size %zu, actual size %jd\n", fd, bufsize, (intmax_t)st.st_size);
  
  *out_fd = fd;
  return buf;
}
