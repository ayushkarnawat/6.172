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

// Implements the ADT specified in bitarray.h as a packed array of bits; a bit
// array containing bit_sz bits will consume roughly bit_sz/8 bytes of
// memory.

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "./bitarray.h"


// ********************************* Types **********************************

// Concrete data type representing an array of bits.
struct bitarray {
  // The number of bits represented by this bit array.
  // Need not be divisible by 8.
  size_t bit_sz;

  // The underlying memory buffer that stores the bits in
  // packed form (8 per byte).
  char* buf;
};


// ******************** Prototypes for static functions *********************

/**
 * @brief Rotates a subarray left by an arbitrary number of bits.
 * 
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 * 
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 * @param bit_left_amount Number of places to rotate the subarray left.
 */
static void bitarray_rotate_left(bitarray_t* const bitarray,
                                 const size_t bit_offset,
                                 const size_t bit_length,
                                 const size_t bit_left_amount);

/**
 * @brief Rotates a subarray left by one bit.
 *
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 */
static void bitarray_rotate_left_one(bitarray_t* const bitarray,
                                     const size_t bit_offset,
                                     const size_t bit_length);

/**
 * @brief Rotates subarray by transforming `ab` to `ba`.
 * 
 * Consider the string to be rotated to be of the form `ab`, where `a` and `b`
 * are bit strings. In order to transform `ab` to `ba`, we copy `a` to an
 * auxiliary array, move `b` to its final location, then copy `a` from the
 * auxiliary array to its final location.
 * 
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 * 
 * NOTE: Large auxiliary array can be problematic for cache performance when
 * rotating long strings.
 * 
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 * @param bit_right_amount Number of places to rotate the subarray right.
 */
static void bitarray_rotate_ab(bitarray_t* const bitarray,
                               const size_t bit_offset,
                               const size_t bit_length,
                               const ssize_t bit_right_amount);

/**
 * @brief Portable modulo operation that supports negative dividends.
 * 
 * Many programming languages define modulo in a manner incompatible with its
 * widely-accepted mathematical definition. http://stackoverflow.com/q/1907565/
 * provides details; in particular, C's modulo operator (which the standard
 * calls a "remainder" operator) yields a result signed identically to the
 * dividend e.g., -1 % 10 yields -1. This is obviously unacceptable for a
 * function which returns size_t, so we define our own.
 * 
 * @param n Dividend.
 * @param m Divisor.
 * @returns Positive integer `r = n (mod m)`, in the range `[0,m)`.
 * @example modulo(3, 5) = 3
 * @example modulo(-1, 10) = 9
 */
static size_t modulo(const ssize_t n, const size_t m);

/**
 * @brief Produces a mask which, when ANDed with a byte, retains only the
 * bit_index th byte.
 *
 * (Note that here the index is counted from right to left, which is different
 * from how we represent bitarrays in the tests.  This function is only used by
 * bitarray_get and bitarray_set, however, so as long as you always use
 * bitarray_get and bitarray_set to access bits in your bitarray, this reverse
 * representation should not matter.
 * 
 * @param bit_index Index which represents bit.
 * @returns Char representation of the mask containing `bit_index`th byte.
 * @example bitmask(5) = 0b00100000.
 */
static char bitmask(const size_t bit_index);


// ******************************* Functions ********************************

bitarray_t* bitarray_new(const size_t bit_sz) {
  // Allocate an underlying buffer of ceil(bit_sz/8) bytes.
  char* const buf = (char*) calloc(1, (bit_sz+7) / 8);
  if (buf == NULL) {
    return NULL;
  }

  // Allocate space for the struct.
  bitarray_t* const bitarray = (bitarray_t*) malloc(sizeof(struct bitarray));
  if (bitarray == NULL) {
    free(buf);
    return NULL;
  }

  bitarray->buf = buf;
  bitarray->bit_sz = bit_sz;
  return bitarray;
}

void bitarray_free(bitarray_t* const bitarray) {
  if (bitarray == NULL) {
    return;
  }
  free(bitarray->buf);
  bitarray->buf = NULL;
  free(bitarray);
}

size_t bitarray_get_bit_sz(const bitarray_t* const bitarray) {
  return bitarray->bit_sz;
}

bool bitarray_get(const bitarray_t* const bitarray, const size_t bit_index) {
  assert(bit_index < bitarray->bit_sz);

  // We're storing bits in packed form, 8 per byte.  So to get the nth
  // bit, we want to look at the (n mod 8)th bit of the (floor(n/8)th)
  // byte.
  //
  // In C, integer division is floored explicitly, so we can just do it to
  // get the byte; we then bitwise-and the byte with an appropriate mask
  // to produce either a zero byte (if the bit was 0) or a nonzero byte
  // (if it wasn't).  Finally, we convert that to a boolean.
  return (bitarray->buf[bit_index / 8] & bitmask(bit_index)) ? true : false;
}

void bitarray_set(bitarray_t* const bitarray,
                  const size_t bit_index,
                  const bool value) {
  assert(bit_index < bitarray->bit_sz);

  // We're storing bits in packed form, 8 per byte.  So to set the nth
  // bit, we want to set the (n mod 8)th bit of the (floor(n/8)th) byte.
  //
  // In C, integer division is floored explicitly, so we can just do it to
  // get the byte; we then bitwise-and the byte with an appropriate mask
  // to clear out the bit we're about to set.  We bitwise-or the result
  // with a byte that has either a 1 or a 0 in the correct place.
  bitarray->buf[bit_index / 8] =
    (bitarray->buf[bit_index / 8] & ~bitmask(bit_index)) |
    (value ? bitmask(bit_index) : 0);
}

void bitarray_randfill(bitarray_t* const bitarray){
  int32_t* ptr = (int32_t*) bitarray->buf;
  for (int64_t i=0; i<bitarray->bit_sz/32 + 1; i++){
    ptr[i] = rand();
  }
}

void bitarray_rotate(bitarray_t* const bitarray,
                     const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t bit_right_amount) {
  assert(bit_offset + bit_length <= bitarray->bit_sz);

  if (bit_length == 0) {
    return;
  }

  // Convert a rotate left or right to a left rotate only, and eliminate
  // multiple full rotations.
  // bitarray_rotate_left(bitarray, bit_offset, bit_length,
  //                      modulo(-bit_right_amount, bit_length));
  bitarray_rotate_ab(bitarray, bit_offset, bit_length,
                     modulo(bit_right_amount, bit_length));
}

static void bitarray_rotate_left(bitarray_t* const bitarray,
                                 const size_t bit_offset,
                                 const size_t bit_length,
                                 const size_t bit_left_amount) {
  for (size_t i = 0; i < bit_left_amount; i++) {
    bitarray_rotate_left_one(bitarray, bit_offset, bit_length);
  }
}

static void bitarray_rotate_left_one(bitarray_t* const bitarray,
                                     const size_t bit_offset,
                                     const size_t bit_length) {
  // Grab the first bit in the range, shift everything left by one, and
  // then stick the first bit at the end.
  const bool first_bit = bitarray_get(bitarray, bit_offset);
  size_t i;
  for (i = bit_offset; i + 1 < bit_offset + bit_length; i++) {
    bitarray_set(bitarray, i, bitarray_get(bitarray, i + 1));
  }
  bitarray_set(bitarray, i, first_bit);
}

static size_t modulo(const ssize_t n, const size_t m) {
  const ssize_t signed_m = (ssize_t)m;
  assert(signed_m > 0);
  const ssize_t result = ((n % signed_m) + signed_m) % signed_m;
  assert(result >= 0);
  return (size_t)result;
}

static char bitmask(const size_t bit_index) {
  return 1 << (bit_index % 8);
}

static void bitarray_rotate_ab(bitarray_t* const bitarray,
                            const size_t bit_offset,
                            const size_t bit_length,
                            const ssize_t bit_right_amount) {
  // Ensure bit shifts to the right only
  assert(bit_length > bit_right_amount && bit_right_amount >= 0);
  // assert(bit_right_amount >= 0);

  // Store bits to move in auxillary array
  bitarray_t* aux = bitarray_new(bit_right_amount);
  size_t tmp_index = 0;

  for (size_t i=bit_offset+(bit_length-bit_right_amount); i < bit_offset+bit_length; i++) {
    // printf("%zu, %zu, %d \n", i, tmp_index, bitarray_get(bitarray, i));
    bitarray_set(aux, tmp_index, bitarray_get(bitarray, i));
    ++tmp_index;
  }

  // Move bits into (new) location
  for (size_t i=bit_offset + (bit_length-bit_right_amount); i > bit_offset; --i) {
    printf("%zu, %zu, %d \n", i, i+bit_right_amount, bitarray_get(bitarray, i));
    bitarray_set(bitarray, i+bit_right_amount, bitarray_get(bitarray, i));
  }
  for (size_t i=0; i < aux->bit_sz; ++i) {
    // printf("%zu, %zu, %d \n", i, i+bit_offset, bitarray_get(aux, i));
    bitarray_set(bitarray, i+bit_offset, bitarray_get(aux, i));
  }
  bitarray_free(aux);

  // for (size_t i = bit_offset; i < bit_offset + bit_length-bit_right_amount; i++) {
  //   size_t new_index = i + bit_right_amount;
  //   if (new_index >= bit_offset + bit_length) {
  //     new_index -= bit_length;
  //   }
  //   // printf("%zu, %d \n", new_index, bitarray_get(bitarray, i));
  //   bitarray_set(bitarray, new_index, bitarray_get(bitarray, i));
  // }
}
