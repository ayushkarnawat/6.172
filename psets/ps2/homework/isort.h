#ifndef ISORT_H
#define ISORT_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t data_t;

/**
 * @brief Insertion sort array from beginning to end, inclusively.
 *
 * @param begin Pointer to first element in array.
 * @param end Pointer to last element in array.
 */
void isort(data_t* begin, data_t* end);

#endif
