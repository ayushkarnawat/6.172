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

#include "bitarray.h"


const uint8_t BYTEFLIP_LOOKUP[16] = {
  0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
  0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
};

// ********************************* Types **********************************

// Concrete data type representing an array of bits.
struct bitarray {
  size_t bit_sz;  // num bits, need not be divisible by 8
  char* buf;      // underlying memory buffer that stores bits in packed form
};


// ******************** Prototypes for static functions *********************

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
 * @brief Rotates a subarray right by one bit.
 *
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 */
static void bitarray_rotate_right_one(bitarray_t* const bitarray,
                                      const size_t bit_offset,
                                      const size_t bit_length);

/**
 * @brief Rotates a subarray right by an arbitrary number of bits.
 *
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 * @param bit_right_amount Number of places to rotate the subarray right.
 */
static void bitarray_rotate_right(bitarray_t* const bitarray,
                                  const size_t bit_offset,
                                  const size_t bit_length,
                                  const size_t bit_left_amount);

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
 * @brief Find first instance of unoccupied index in the bitarray.
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @returns Unoccupied index; -1 if none are found
 */
static long find_unoccupied_idx(const bitarray_t* const bitarray);

/**
 * @brief Check if all the bits are in their final positions (all 1s).
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @returns true if all bits are 1s; false otherwise.
 */
inline static bool is_final(const bitarray_t* const bitarray);

/**
 * @brief Rotates subarray by moving each bit to its specified location
 * directly.
 *
 * Keep the temporay value of the bit along with its location between every
 * iteration. Move the bit stored in this temporary index to its new location.
 * Space complexity: O(1). Time complexity: O(bit_length).
 *
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 *
 * NOTE: Although constant auxillary space is used, the memory accesses are
 * scattered, which can adversely impact caching.
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 * @param bit_right_amount Number of places to rotate the subarray right.
 */
static void bitarray_rotate_cyclic(bitarray_t* const bitarray,
                                   const size_t bit_offset,
                                   const size_t bit_length,
                                   const ssize_t bit_right_amount);

/**
 * @brief Reverses the bitarray bit by bit.
 *
 * NOTE: If a subarray is greater than a byte in length, then constantly
 * swapping bits naively is bad for performance since we will be constantly
 * accessing bits from different buffers (aka non-contiguous memory access).
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 */
static void bitarray_reverse_bit(bitarray_t* const bitarray,
                                 const size_t bit_offset,
                                 const size_t bit_length);

/**
 * @brief Rotates the subarray by performing reverse operations.
 *
 * Consider the string to be rotated to be of the form `ab`, where `a` and `b`
 * are bit strings. Observe the identity `(a^R b^R)^R = ba`, where `R` is the
 * operation that reverses a string. The "reverse" operation can be
 * accomplished using only constant storage. Thus, with 3 reversals of bit
 * strings, the string can be rotated.
 *
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length). That is, the start is inclusive, but the end is exclusive.
 *
 * @param bitarray Pointer to bitarray to be rotated.
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 */
static void bitarray_reverse(bitarray_t* const bitarray,
                             const size_t bit_offset,
                             const size_t bit_length);


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

static char bitmask(const size_t bit_index) {
  return 1 << (bit_index % 8);
}

static size_t modulo(const ssize_t n, const size_t m) {
  const ssize_t signed_m = (ssize_t)m;
  assert(signed_m > 0);
  const ssize_t result = ((n % signed_m) + signed_m) % signed_m;
  assert(result >= 0);
  return (size_t)result;
}

static void bitarray_rotate_right_one(bitarray_t* const bitarray,
                                      const size_t bit_offset,
                                      const size_t bit_length) {
  // Grab the last bit in the bitarray, shift everything right by one,
  // and then stick the last bit at the front.
  bool last_bit = bitarray_get(bitarray, bit_offset+bit_length-1);
  for (size_t i = bit_offset+bit_length-1; i > bit_offset; i--) {
    bitarray_set(bitarray, i, bitarray_get(bitarray, i-1));
  }
  bitarray_set(bitarray, bit_offset, last_bit);
}

static void bitarray_rotate_right(bitarray_t* const bitarray,
                                  const size_t bit_offset,
                                  const size_t bit_length,
                                  const size_t bit_right_amount) {
  for (size_t i = 0; i < bit_right_amount; i++) {
    bitarray_rotate_right_one(bitarray, bit_offset, bit_length);
  }
}

static void bitarray_rotate_ab(bitarray_t* const bitarray,
                               const size_t bit_offset,
                               const size_t bit_length,
                               const ssize_t bit_right_amount) {
  // Ensure bit shifts to the right only
  assert(bit_length > bit_right_amount);
  assert(bit_right_amount >= 0);

  // Store bits to move in auxillary array
  const size_t seperator = bit_offset + (bit_length - bit_right_amount);
  bitarray_t* aux = bitarray_new(bit_right_amount);
  size_t tmp_index = 0;
  for (size_t i=seperator; i < bit_offset+bit_length; i++) {
    bitarray_set(aux, tmp_index, bitarray_get(bitarray, i));
    ++tmp_index;
  }

  // Move bits into (new) location
  for (size_t i=seperator-1; i >= bit_offset; i--) {
    bitarray_set(bitarray, i+bit_right_amount, bitarray_get(bitarray, i));
    if (i==0) { break; } // cumbersome; prevent bad memory access
  }
  for (size_t i=0; i < aux->bit_sz; ++i) {
    bitarray_set(bitarray, i+bit_offset, bitarray_get(aux, i));
  }

  bitarray_free(aux);
}

static long find_unoccupied_idx(const bitarray_t* const bitarray) {
  const size_t bit_sz = bitarray->bit_sz;
  const size_t buf_sz = bit_sz / 8;
  const size_t num_extra_bits = bit_sz % 8;

  // Fast checking (using 8 bit buffers) if some bits within buffers have 0s.
  for (size_t i=0; i < buf_sz; i++) {
    if ((uint8_t)bitarray->buf[i] != (uint8_t) 0xFF) {
      long current_bit = i*8;
      while (bitarray_get(bitarray, current_bit))
        ++current_bit;
      return current_bit;
    }
  }

  // Test if any remainings bits are unoccupied (aka bit value is 0)
  if (num_extra_bits > 0) {
    for (long i=buf_sz*8; i<bit_sz; i++) {
      if (!bitarray_get(bitarray, i))
        return i;
    }
  }
  return -1;
}

inline static bool is_final(const bitarray_t* const bitarray) {
  return (find_unoccupied_idx(bitarray) == -1) ? true : false;
}

static void bitarray_rotate_cyclic(bitarray_t* const bitarray,
                                   const size_t bit_offset,
                                   const size_t bit_length,
                                   const ssize_t bit_right_amount) {
  // Are the bits in their final position? Initially, all bits are in incorrect
  // positions (represented by 0s). As the bits get placed into their correct
  // positions, they become 1.
  bitarray_t* positions = bitarray_new(bit_length);

  size_t old_index = bit_offset;
  size_t new_index;
  bool old_bit = bitarray_get(bitarray, old_index);
  bool new_bit;
  while (!is_final(positions)) {
    // Determine new position of the bit
    new_index = old_index + bit_right_amount;
    if (new_index >= bit_offset + bit_length) {
      new_index -= bit_length;
    }
    // Store bit value; used to replace bit value at new index 
    new_bit = bitarray_get(bitarray, new_index);
    if (old_index != new_index && !bitarray_get(positions, new_index-bit_offset)) {
      bitarray_set(bitarray, new_index, old_bit);
      bitarray_set(positions, new_index-bit_offset, true);
      old_index = new_index;
      old_bit = new_bit;
    } else {
      // Find another index that needs to be moved. We use a the first index
      // that is not in its final location.
      long unoccupied_idx = find_unoccupied_idx(positions);
      if (unoccupied_idx >= 0) {
        old_index = bit_offset + unoccupied_idx;
        old_bit = bitarray_get(bitarray, old_index);
      }
    }
  }
  bitarray_free(positions);
}

static void bitarray_reverse_bit(bitarray_t* const bitarray,
                                 const size_t bit_offset,
                                 const size_t bit_length) {
  // Sanity check
  assert(bit_offset + bit_length <= bitarray_get_bit_sz(bitarray));

  bool temp;
  size_t start = bit_offset;
  size_t end = bit_offset + bit_length - 1;
  while (start < end) {
    // Swaps the start and end bit
    temp = bitarray_get(bitarray, start);
    bitarray_set(bitarray, start, bitarray_get(bitarray, end));
    bitarray_set(bitarray, end, temp);
    start++;
    end--;
  }
}

static uint8_t reverse_byte(uint8_t bitarray) {
  // Uses last 8 bits; if len of bitarray is less than 8 bits, assumes the
  // higher bits are 0s.
  return (BYTEFLIP_LOOKUP[bitarray&0b1111] << 4) | BYTEFLIP_LOOKUP[bitarray>>4];
}

static void bitarray_reverse(bitarray_t* const bitarray,
                             const size_t bit_offset,
                             const size_t bit_length) {
  // Manually reverse bits if within a single byte
  if (bit_length <= 8) {
    bitarray_reverse_bit(bitarray, bit_offset, bit_length);
    return;
  }

  // Calculate indices of the partial bytes one each end of the substring.
  // These bits are from index left_start (inclusive) to left_start+left_len
  // (exclusive) and from right_start to right_start+right_len.

  // Both the right_byte and left_byte can be atmost 8 bit bytes buffer. The
  // ending bit index for left will always be at a multiple of 8; easier buffer
  // access (without needing to access unnecessary bits from next buffer). 
  // (8 - 10) % 8 = 6
  // (10 + 21) % 8 = 5
  size_t left_start = bit_offset;
  size_t left_byte_length = (8 - bit_offset) % 8;
  size_t right_byte_length = (bit_offset + bit_length) % 8; 
  size_t right_start = bit_offset + bit_length - right_byte_length;

  // Reverse the order of bits within each byte
  size_t byte_start = (left_start + left_byte_length) / 8;
  size_t byte_end = (right_start / 8); // exclusive
  /// TODO: Check performance of access below? Does it result in cache miss,
  /// since byte size is bigger than cache line?
  uint8_t* left_byte = (uint8_t*) (bitarray->buf + byte_start);
  uint8_t* right_byte = (uint8_t*) (bitarray->buf + (byte_end-1));
  uint8_t temp;

  // Ref:
  // https://github.com/albertyw/6.172/blob/master/project1/everybit/bitarray.c

  // Rather, instead of reversing bit by bit, it is better to reverse the 8-bit
  // buffers and leave the remaining bits to be reversed bit by bit. For
  // example, if a buffer is 0b11101011, then its reverse will be 0b11010111.
  // We save these buffer values and their complement in a lookup table and
  // access them using the complement when needed. When replacing the buffers,
  // we store the whole buffer's complement in an auxillary array, move the old
  // buffer to its new location, and the new buffer to the old buffers
  // location. We can then move this buffer altogether to its final position.
}

void bitarray_rotate(bitarray_t* const bitarray,
                     const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t bit_right_amount) {
  assert(bit_offset + bit_length <= bitarray->bit_sz);

  // Don't do anything if there's nothing to rotate
  if (bit_length <= 1)
    return;

  // Converts rotates in either direction to a right rotate
  size_t k = modulo(bit_right_amount, bit_length);

  // Don't do anything if it's not being rotated
  if (k == 0)
    return;

  // bitarray_rotate_right(bitarray, bit_offset, bit_length, k);
  // bitarray_rotate_ab(bitarray, bit_offset, bit_length, k);
  bitarray_rotate_cyclic(bitarray, bit_offset, bit_length, k);

  // Rotate using bit reverse
  // bitarray_reverse(bitarray, bit_offset, bit_length - k);
  // bitarray_reverse(bitarray, bit_offset + bit_length - k, k);
  // bitarray_reverse(bitarray, bit_offset, bit_length);
}
