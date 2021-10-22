/**
 * Copyright (c) 2012 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include "./isort.h"
#include "./util.h"


// ******************** Prototypes for static functions *********************

// A merge routine. Merges the sub-arrays A [start...mid] and A [mid+1 ... end].
// Allocates extra scratch space 'left' to merge the subarrays.
static inline void merge_m(data_t* arr, int start, int mid, int end);

// Copy values from source to destination.
static inline void copy_m(data_t* src, data_t* dest, int n);

// ******************************* Functions ********************************

void sort_m(data_t* arr, int start, int end) {
  assert(arr);
  // In practice, merge sort is slow for small array sizes. Using faster sorting
  // techniques (i.e. insertion sort) when the array size is small (aka < 100),
  // can make a significant improvement in the runtime of the algorithm.
  if (end-start < 100){
    isort(&(arr[start]), &(arr[end]));
  } else {
    int mid = (start + end) / 2;
    sort_m(arr, start, mid);
    sort_m(arr, mid + 1, end);
    merge_m(arr, start, mid, end);
  }
}

static inline void merge_m(data_t* arr, int start, int mid, int end) {
  assert(arr);
  assert(start <= mid);
  assert((mid + 1) <= end);
  int n_left = mid - start + 1;
  int n_right = end - mid;

  data_t* left = 0;
  mem_alloc(&left, n_left+1);
  if (left == NULL) {
    mem_free(&left);
    return;
  }

  copy_m(&(arr[start]), left, n_left);
  left[n_left] = UINT_MAX;

  unsigned int* arrptr = &(arr[start]);
  unsigned int* leftptr = left;
  unsigned int* rightptr = &(arr[mid+1]);

  while (n_left > 0 && n_right > 0) {
    long cmp = (*leftptr <= *rightptr);
    long min = *rightptr ^ ((*leftptr ^ *rightptr) & -(cmp));

    *arrptr++ = min;
    leftptr += cmp; n_left -= cmp;
    rightptr += !cmp; n_right -= !cmp;
  }

  while (n_left > 0) {
    *arrptr++ = *leftptr;
    n_left--;
  }

  mem_free(&left);
}

static inline void copy_m(data_t* src, data_t* dest, int n) {
  assert(src);
  assert(dest);

  // access use ptrs instead of array idxs
  for (int i = 0 ; i < n ; i++) {
    *(dest+i) = *(src+i);
  }
}
