/**
 * Parallel quicksort.
 * 
 * https://www.uio.no/studier/emner/matnat/ifi/INF3380/v18/undervisningsmateriale/
 * https://www.uio.no/studier/emner/matnat/ifi/INF3380/v18/timeplan/index.html#2-1
 * 
 * https://www.uio.no/studier/emner/matnat/ifi/INF3380/v18/undervisningsmateriale/quicksort.c
 * https://www.uio.no/studier/emner/matnat/ifi/INF3380/v10/undervisningsmateriale/inf3380-week12.pdf
 * https://www.uio.no/studier/emner/matnat/ifi/INF3380/v17/undervisningsmateriale/chap9_selected_slides2014.pdf
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Swaps two elements (via their pointers).
 * 
 * @param a First element.
 * @param b Second element.
 */
void swap(int* a, int* b) {
  int t = *a;
  *a = *b;
  *b = t;
}

/**
 * @brief Create a random list of `n` ints, uniformly sampled between `[0,n)`.
 * 
 * @param array Pointer to pointer of array.
 */
void random_array(int **array, int n) {
  int* arr = (int*) malloc(n * sizeof(int));
  if(arr == NULL) {
    fprintf(stderr, "Could not malloc, exiting.\n");
    exit(-1);
  }

  for(int i=0; i<n; i++)
    arr[i] = rand() % n; // choose uniformly between [0,n]
  *array = arr;
}

/**
 * @brief Find the optimal pivot.
 *
 * For large patitions, choosing the median of the first, middle, and last
 * element of the partition allows for the selection of the optimal pivot,
 * according to Sedgewick[1]. This rule counters the case of sorted (or
 * reverse-sorted) input, and gives a better estimate of the optimal pivot (the
 * true median) than selecting any single element, when no information about the
 * ordering of the input is known.
 *
 * @param a Pointer to partition.
 * @param n Length of partition.
 * @returns Optimal pivot index.
 *
 * @ref{1} Sedgewick, Robert Algorithms in C: Fundamentals, Data Structures,
 *          Sorting, Searching, Parts 1–4.
 */
int optimal_pivot(int *a, int n) {
  if ((a[n/2]>a[0] && a[0]>a[n-1]) || (a[n-1]>a[0] && a[0]>a[n/2])) {
    return 0;
  }
  if ((a[0]>a[n/2] && a[n/2]>a[n-1]) || (a[n-1]>a[n/2] && a[n/2]>a[0])) {
    return n/2;
  }
  return n-1;
}

/**
 * Recursive Quicksort.
 *
 * The steps are as follows:
 * - Pick a pivot from the list of sorted numbers.
 * - Put all the numbers lower than that pivot's value into the left subarray,
 *   and all the values higher than the pivot into the right subarray. 
 * - Recursively sort each subarray. 
 *
 * Some caveats with this approach: what happens if the pivot chosen leads to an
 * inbalance in the left and right subarrays? In order words, how do we enforce
 * better load balancing? One way is to choose a pivot value close to the true
 * median value from the list. 
 *
 * @param arr Array to be sorted.
 * @param high Ending index.
 */
void local_qsort(int* arr, int high) {
  if (high == 1)
    return;
  if (high == 2) {
    if (arr[0] > arr[1])
      swap(&arr[0], &arr[1]);
    return;
  }

  // Find pivot (within each subarray)
  int idx = optimal_pivot(arr, high);
  int pivot = arr[idx];
  // printf("%d, %d \n", idx, pivot);

  int i = quicksort_one_step(arr, high, pivot);
  local_qsort(&arr[0], i);
  local_qsort(&arr[i], high-i);
}

// one step quicksort to use in parallel process
int quicksort_one_step(int* arr, int high, int pivot) {
  int i = 0;
  int j = high-1;
  while(i <= j) {
    while(arr[i] < pivot)
      i++;
    while(arr[j] > pivot)
      j--;
    if(i <= j) {
      swap(&arr[i], &arr[j]);
      i++;
      j--;
    }
  }
  return i;
}

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  int *arr;
  
  random_array(&arr, n);
  for (int i=0; i<n; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");

  local_qsort(arr, n);
  for (int i=0; i<n; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");

  free(arr);
}
