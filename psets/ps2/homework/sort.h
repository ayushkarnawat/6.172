// Copyright (c) 2012 MIT License by 6.172 Staff

#ifndef SORT_H
#define SORT_H

#include "./util.h"

/**
 * @brief Basic merge sort; sorts the array A[start...end].
 *
 * @param arr Pointer to array to be sorted.
 * @param start Starting index.
 * @param end End index.
 */
void sort_a(data_t* arr, int start, int end);

/**
 * @brief Merge sort with inline optimization.
 *
 * @param arr Pointer to array to be sorted.
 * @param start Starting index.
 * @param end End index.
 */
void sort_i(data_t* arr, int start, int end);
void sort_p(data_t* arr, int start, int end);
void sort_c(data_t* arr, int start, int end);
void sort_m(data_t* arr, int start, int end);
void sort_f(data_t* arr, int start, int end);

#endif  // SORT_H
