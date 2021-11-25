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

#ifndef TESTS_H
#define TESTS_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#include "./bitarray.h"


// ******************************* Prototypes *******************************

/**
 * @brief Runs increasingly larger test cases (defined by a tier num), until a
 * test case takes longer than time_limit_seconds to complete.
 *
 * @param time_limit_seconds Maximum allowed time.
 * @return Tier number which was last successful.
 */
int timed_rotation(const double time_limit_seconds);

/**
 * @brief Runs the testsuite specified in a given file.
 *
 * @param filename Path to test file.
 * @param selected_test Which specified test to run.
 */
void parse_and_run_tests(const char* filename, int selected_test);

/**
 * TODO: Remove. For testing purposes only.
 */
void sample_test_a();

#endif  // TESTS_H
