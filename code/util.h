#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <x86intrin.h>

#define INTERVAL 1<<15
#define CACHE_MISS_LATENCY 100

#define SHARED_FILE "/bin/ls"

#endif
