// MIT License
//
// Copyright (c) 2025 Stew Forster (stew675@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define TEST_SIZE       5       // Number of items in array we're sorting
#define HISTOGRAM_SIZE  16      // Number of items in histogram table

static  size_t  total_comps = 0, total_swaps = 0;
static  size_t  num_comps = 0, num_swaps = 0;
static  double  total_sorts = 0;
static  size_t  histogram[HISTOGRAM_SIZE] = {0};

// Returns boolean result of *pa < *pb.  Counts comparisons
static bool
is_lt(int *pa, int *pb)
{
        num_comps++;
        return (*pa < *pb);
} // is_lt

// Swaps two integers pointed at by *pa and *pb.  Counts swaps
static void
swap(int *pa, int *pb)
{
        num_swaps++;
        int t = *pa;
        *pa = *pb;
        *pb = t;
} // swap


// Sorts 5 items in at most 7 comparisons (or less)
// This solution also averages just 4.6 swaps by centralising unsorted elements
static void
sort5in7(int *p1, int *p2, int *p3, int *p4, int *p5)
{
        bool p1_was_swapped_with_p3 = false;

        // Sort p1 and p2
        if (is_lt(p2, p1))
                swap(p1, p2);   // (p1, p2) | p3 | p4 | p5

        // Sort p4 and p5
        if (is_lt(p5, p4))
                swap(p4, p5);   // (p1, p2) | p3 | (p4, p5)

        // Establish a set of three sorted items at positions
        // P1, P4, and P5.  We'll handle placing P2 later on
        if (is_lt(p4, p1)) {
                swap(p1, p4);
                swap(p2, p5);
        }

        // After the above, we know: P1 <= P4 <= P5, AND P1 <= P2
        // Note that we do NOT know if P2 <= P4 or if P2 <= P5 yet
        // Where P3 must go is not known at all at this time

        // Now place P3 in order with (P1,P4,P5) using a binary search.
        // This will always take two comparisons.
        if (is_lt(p4, p3)) {
                swap(p3, p4);
                if (is_lt(p5, p4))
                        swap(p4, p5);
        } else {
                // Take note of if P3 < P1, as this will move
                // P1 past P2 into P3, and we can save on a
                // comparison later if we record this result
                if (is_lt(p3, p1)) {
                        swap(p1, p3);
                        p1_was_swapped_with_p3 = true;
                }
        }

        // We now know that:  P1 <= P3 <= P4 <= P5, AND P1 <= P2
        // Additionally if P1 was swapped with P3 above, then we
        // also know that P3 <= P2
        // Now we insert P2 into P3/P4/P5 using a binary search.
        if (is_lt(p4, p2)) {
                // Shift P2 up to P4
                swap(p2, p3);   // Satisfies the p1_was_swapped_with_p3 case
                swap(p3, p4);
                // Now test if we need to move P4 to P5
                if (is_lt(p5, p4))
                        swap(p4, p5);
        } else {
                // We need to check if P2 moves to before or after P3
                // If P1 was swapped with P3 earlier then we can just swap
                // P2 and P3 without doing a comparison, otherwise we need
                // to check if P3 < P2 before swapping
                if (p1_was_swapped_with_p3 || is_lt(p3, p2))
                        swap(p2, p3);
        }
} // sort5in7

static void
print_array(int pa[], size_t n)
{
        printf("[ %2d", pa[0]);
        for (size_t i = 1; i < n - 1; i++)
                printf(", %2d", pa[i]);
        printf(", %2d]\n", pa[n - 1]);
} // print_array

static void
validate(int pa[], int cpa[], size_t n)
{
        bool    sort_failed = false;

        // Check if sort exceeded 7 comparisons
        if (num_comps > 7) {
                printf("\nSORT USED GREATER THAN 7 COMPARISONS: ");
                printf("%lu compares\n", num_comps);
                sort_failed = true;
        }

        if (num_comps < HISTOGRAM_SIZE) {
                histogram[num_comps]++;
        }

        // Check if sorted items are out of order
        size_t  i;
        for (i = n - 1; i > 0; i--)
                if (cpa[i] < cpa[i-1])
                        break;

        if (i) {
                printf("\nSORT FAILED\n");
                sort_failed = true;
        }

        // Print input and output arrays if required
        if (sort_failed) {
                printf("INPUT ARRAY  -> ");
                print_array(pa, n);
                printf("OUTPUT ARRAY -> ");
                print_array(cpa, n);
                printf("\n");
        }
} // validate

// Receives an array permutation and invokes the actual
// sort function. Validates the result after it's sorted
static void
call_sort(int pa[], const int n)
{
        int cpa[TEST_SIZE];   // Holds copy of items to sort

        // Copy the permuted array before sorting as we do
        // not want to sort what permute() is working with
        for (size_t i = 0; i < n; i++) cpa[i] = pa[i];

        // Reset swap and comp counters back to 0
        num_comps = 0;
        num_swaps = 0;

        // Call our sort function with the (copied) permutation
        sort5in7(&cpa[0], &cpa[1], &cpa[2], &cpa[3], &cpa[4]);

        // Validate that it's correct
        validate(pa, cpa, n);

        total_comps += num_comps;
        total_swaps += num_swaps;
        total_sorts += 1;
} // call_sort

static void
permute(int a[], int pos, int n)
{
        assert(pos < TEST_SIZE);

        if (pos >= (n - 1))
                return call_sort(a, n);

        for (int i = pos, t; i < n; i++) {
                // Swap current element
                t = a[pos]; a[pos] = a[i]; a[i] = t;

                // Recurse to permute the rest of the array
                permute(a, pos + 1, n);

                // Undo what we swapped
                t = a[pos]; a[pos] = a[i]; a[i] = t;
        }
} // permute

int
main()
{
        int     p[TEST_SIZE] = {0};

        printf("\nTest Sort Size is %d items\n\n", (int)TEST_SIZE);

        // Initialise the permute test array
        for (int i = 0; i < TEST_SIZE; i++)
                p[i] = i + 1;

        permute(p, 0, TEST_SIZE);

        printf("     Comparison Histogram\n");
        printf(" ┏━━━━━━━━━━━━━┳━━━━━━━━━━━━━┓\n");
        printf(" ┃ Comparisons ┃    Count    ┃\n");
        printf(" ┣━━━━━━━━━━━━━╋━━━━━━━━━━━━━┫\n");
        int     first_emitted = 1;
        for (int i = 0; i < HISTOGRAM_SIZE; i++)
                if (histogram[i] > 0) {
                        if (first_emitted == 0) {
                                printf(" ┠─────────────╂─────────────┨\n", i, histogram[i]);
                        }
                        printf(" ┃   %4lu      ┃    %4lu     ┃\n", i, histogram[i]);
                        first_emitted = 0;
                }
        printf(" ┗━━━━━━━━━━━━━┻━━━━━━━━━━━━━┛\n\n");

        printf("total_sorts = %7.0f,  avg. comps = %7.3f,  avg. swaps = %7.3f\n\n",
                total_sorts, total_comps/total_sorts, total_swaps/total_sorts);

} // main
