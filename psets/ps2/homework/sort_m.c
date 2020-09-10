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

#include "./isort.c"
#include "./util.h"


/**
 * A merge routine. Merges the sub-arrays A [start...mid] and A [mid+1 ... end].
 * Uses two arrays 'left' and 'right' in the merge operation.
 * 
 * Instead of using two different arrays (left + right) in the merge operation,
 * we will instead use the original array along with the left subarray to merge
 * the subarrays.
 */
static inline void merge_m(data_t* A, int p, int q, int r);

// Copy values from source to destination. 
static inline void copy_m(data_t* source, data_t* dest, int n);


// A basic merge sort routine that sorts the subarray A[p..r]
void sort_m(data_t* A, int p, int r) {
  assert(A);
  // In practice, merge sort is slow for small array sizes. As such, using
  // faster sorting techniques (i.e. insertion sort) when the array size is
  // small (aka < 100), can make a significant improvement in the runtime of the
  // algorithm.
  if (r-p < 100){
    isort(&(A[p]), &(A[r]));
  } else {
    int q = (p + r) / 2;
    sort_m(A, p, q);
    sort_m(A, q + 1, r);
    merge_m(A, p, q, r);
  }
}

static inline void merge_m(data_t* A, int start, int mid, int end) {
  assert(A);
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

  copy_m(&(A[start]), left, n_left);
  left[n_left] = UINT_MAX;

  unsigned int* Aptr = &(A[start]);
	unsigned int* leftptr = left;
	unsigned int* rightptr = &(A[mid+1]);

  while (n_left > 0 && n_right > 0) {
    long cmp = (*leftptr <= *rightptr);
	  long min = *rightptr ^ ((*leftptr ^ *rightptr) & -(cmp));

    *Aptr++ = min;
	  leftptr += cmp; n_left -= cmp;
	  rightptr += !cmp; n_right -= !cmp;
  }

  while (n_left > 0) {
	  *Aptr++ = *leftptr;
	  n_left--;
	}
  
  mem_free(&left);
}

static inline void copy_m(data_t* source, data_t* dest, int n) {
  assert(dest);
  assert(source);

  // access use ptrs instead of array idxs
  for (int i = 0 ; i < n ; i++) {
    *(dest+i) = *(source+i);
  }
}
