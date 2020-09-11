// Copyright (c) 2015 MIT License by 6.172 Staff

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define SIZE (1L << 16)

void test(uint8_t * restrict a, uint8_t * restrict b) {
  a = __builtin_assume_aligned(a, 16);
  b = __builtin_assume_aligned(b, 16);

  // Find the (row-wise) max of both arrays
  for (uint64_t i = 0; i < SIZE; i++) {
    a[i] = (b[i] > a[i]) ? b[i] : a[i];
  }
}
