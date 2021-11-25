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

#ifndef BITARRAY_H
#define BITARRAY_H

#include <stdbool.h>
#include <sys/types.h>

// ********************************* Types **********************************

// Abstract data type representing an array of bits.
typedef struct bitarray bitarray_t;

// ******************************* Prototypes *******************************

/**
 * @brief Allocates space for a bit array.
 *
 * @param bit_sz The number of bits storable in the resultant bit array.
 * @return Bitarray struct representing an array of bits.
 */
bitarray_t* bitarray_new(const size_t bit_sz);

/**
 * @brief Frees a bit array allocated by bitarray_new.
 *
 * @param bitarray Pointer to a bit array.
 */
void bitarray_free(bitarray_t* const bitarray);

/**
 * Note: the invariant bitarray_get_bit_sz(bitarray_new(n)) = n.
 *
 * @param bitarray Pointer to a bit array.
 * @return Number of bits stored in a bit array.
 */
size_t bitarray_get_bit_sz(const bitarray_t* const bitarray);

/**
 * @brief Indexes into a bitarray, retrieving the bit at the specified index.
 *
 * @param bitarray Pointer to a bitarray.
 * @param bit_index Zero-based index.
 * @return Value of bit at the specified bit_index.
 */
bool bitarray_get(const bitarray_t* const bitarray, const size_t bit_index);

/**
 * @brief Indexes into a bitarray, setting the bit at the specified index.
 *
 * @param bitarray Pointer to a bitarray.
 * @param bit_index Zero-based index.
 * @param value Value of bit.
 */
void bitarray_set(bitarray_t* const bitarray,
                  const size_t bit_index,
                  const bool value);

/**
 * @brief Randomly fill all bits in the bitarray.
 *
 * @param bitarray Pointer to a bitarray.
 */
void bitarray_randfill(bitarray_t* const bitarray);

/**
 * @brief Rotates a subarray.
 *
 * The subarray spans the half-open interval [bit_offset, bit_offset +
 * bit_length); that is, the start is inclusive, but the end is exclusive.
 *
 * Note: bit_right_amount can be negative, in which case a left rotation is
 * performed.
 *
 * @param bit_offset Index of the start of the subarray.
 * @param bit_length Length of the subarray, in bits.
 * @param bit_right_amount Number of places to rotate the subarray right.
 *
 * @example Let ba be a bitarray containing the byte 0b10010110; then,
 * bitarray_rotate(ba, 0, bitarray_get_bit_sz(ba), -1) left-rotates the entire
 * bitarray in place. After the rotation, ba contains the byte 0b00101101.
 *
 * @example Let ba be a bitarray containing the byte 0b10010110; then,
 * bitarray_rotate(ba, 2, 5, 2) rotates the third through seventh (inclusive)
 * bits right two places. After the rotation, ba contains the byte 0b10110100.
 */
void bitarray_rotate(bitarray_t* const bitarray,
                     const size_t bit_offset,
                     const size_t bit_length,
                     const ssize_t bit_right_amount);

#endif  // BITARRAY_H
