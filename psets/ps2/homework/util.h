// Copyright (c) 2012 MIT License by 6.172 Staff

#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t data_t;

void mem_alloc(data_t** space, int size);
void mem_free(data_t** space);

#endif  // UTIL_H
