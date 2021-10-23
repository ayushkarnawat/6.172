#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Typedefs */
typedef uint32_t data_t;

// Swaps two elements
void swap(data_t* a, data_t* b) {
  data_t t = *a;
  *a = *b;
  *b = t;
}

// This function is same in both iterative and recursive.
int partition(data_t arr[], int l, int h) {
  data_t x = arr[h];
  int i = (l - 1);

  for (int j = l; j <= h - 1; j++) {
    if (arr[j] <= x) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[h]);
  return (i + 1);
}

/**
 * @brief Iterative quicksort.
 *
 * @param arr Array to be sorted.
 * @param low Starting index.
 * @param high Ending index.
 */
void quickSortIterative(data_t arr[], int low, int high) {
  // Create an auxiliary stack
  int stack[high - low + 1];

  // initialize top of stack
  int top = -1;

  // push initial values of low and high to stack
  stack[++top] = low;
  stack[++top] = high;

  // Keep popping from stack while is not empty
  while (top >= 0) {
    // Pop high and low
    high = stack[top--];
    low = stack[top--];

    // Set pivot element at its correct position
    // in sorted array
    int p = partition(arr, low, high);

    // If there are elements on left side of pivot,
    // then push left side to stack
    if (p - 1 > low) {
      stack[++top] = low;
      stack[++top] = p - 1;
    }

    // If there are elements on right side of pivot,
    // then push right side to stack
    if (p + 1 < high) {
      stack[++top] = p + 1;
      stack[++top] = high;
    }
  }
}
