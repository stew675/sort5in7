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

#define MAX_PERMUTE     20      // Maximum number of items we will permute
#define TEST_SIZE       5       // Number of items in array we're sorting

static size_t   num_comps = 0, num_swaps = 0;
static double   num_sorts = 0;
static void     swap(void *, void *);
static bool     is_lt(void *, void *);          // True if first is less than second


// Sorts 5 items in at most 7 comparisons (or less)
static void
sort5in7(int *p1, int *p2, int *p3, int *p4, int *p5)
{
        bool p1_was_moved = false;

        // Sort p1 and p2
        if (is_lt(p2, p1))
                swap(p1, p2);   // (p1, p2) | p3 | p4 | p5

        // Sort p3 and p4
        if (is_lt(p4, p3))
                swap(p3, p4);   // (p1, p2) | (p3, p4) | p5

        // Establish a set of three sorted items at positions
        // P1, P3, and P4.  We'll handle placing P2 later on
        if (is_lt(p3, p1)) {
                swap(p1, p3);
                swap(p2, p4);
        }

        // After the above, we know: P1 < P3 < P4, AND P1 < P2
        // Note that we do NOT know if P2 < P3 yet

        // Now place P5 in (P1,P3,P4) with a binary search.
        // This will take two comparisons.
        if (is_lt(p5, p3)) {
                // Take note of if P5 < P1, as this will move
                // P1 past P2 into P3, and we can save on a
                // comparison later if we record this result
                if ((p1_was_moved = is_lt(p5, p1)))
                        swap(p1, p5);
                swap(p3, p5);
                swap(p4, p5);
        } else {
                if (is_lt(p5, p4))
                        swap(p4, p5);
        }

        // We now have:  P1 < P3 < P4 < P5, AND P1 < P2.  Place
        // P2 into P3/P4/P5 using a binary search.  This can use
        // less than two comparisons if p1_was_moved is true
        if (is_lt(p4, p2)) {
                // Move P2 up to P4
                swap(p2, p4);
                swap(p2, p3);
                // Now test if we need to move P4 to P5
                if (is_lt(p5, p4))
                        swap(p4, p5);
        } else {
                // If P1 moved to P3, (and P3 moved to P4) and
                // since we checked P2 < P4 above, we can skip
                // a comparison here under that circumstance
                if (p1_was_moved || is_lt(p3, p2))
                        swap(p2, p3);
        }
} // sort5in7


static bool
is_lt(void *va, void *vb)
{
        num_comps++;
        return (*(int *)va < *(int *)vb);
} // is_lt

static void
swap(void *va, void *vb)
{
        num_swaps++;
        int t = *(int *)va;
        *(int *)va = *(int *)vb;
        *(int *)vb = t;
} // swap

static void
print_array(int *a, size_t n)
{
        printf("[ %2d", a[0]);
        for (size_t i = 1; i < n - 1; i++)
                printf(", %2d", a[i]);
        printf(", %2d]\n", a[n - 1]);
} // print_array

static void
validate(int *a, int *t, size_t n)
{
        size_t  i;

        for (i = n - 1; i > 0; i--)
                if (t[i] < t[i-1])
                        break;

        if (i) {
                printf("\nSORT FAILED\n");
                printf("INPUT ARRAY  -> ");
                print_array(a, n);
                printf("OUTPUT ARRAY -> ");
                print_array(t, n);
                printf("\n");
        }
} // validate

// Permutation helper that doesn't count swaps
static inline void
pswap(void *va, void *vb)
{
        int t = *(int *)va;
        *(int *)va = *(int *)vb;
        *(int *)vb = t;
} // pswap

static void
permute(int a[], int pos, int n, const void (*_func)(void *, const int))
{
        assert(pos < MAX_PERMUTE);

        if (pos >= (n - 1))
                return _func(a, n);

        for (int i = pos; i < n; i++) {
                // Swap current element
                pswap(&a[pos], &a[i]);

                // Recurse to permute the rest of the array
                permute(a, pos + 1, n, _func);

                // Undo what we swapped
                pswap(&a[pos], &a[i]);
        }
} // permute

// Receives an array permutation and invokes the actual
// sort function. Validates the result after it's sorted
static void
call_sort(void *va, const int n)
{
        int *a = (int *)va;
        int cpa[MAX_PERMUTE];   // Holds copy of items to sort

        // Copy the permuted array before sorting as we do
        // not want to sort what permute() is working with
        for (size_t i = 0; i < n; i++) cpa[i] = a[i];

        // Call our sort function with the (copied) permutation
        sort5in7(&cpa[0], &cpa[1], &cpa[2], &cpa[3], &cpa[4]);

        // Validate that it's correct
        validate(a, cpa, n);

        num_sorts++;
} // do_sort

int
main()
{
        int     p[TEST_SIZE] = {0};

        printf("\nTest Sort Size is %d items\n\n", (int)TEST_SIZE);

        // Initialise the permute test array
        for (int i = 0; i < TEST_SIZE; i++)
                p[i] = i + 1;

        permute(p, 0, TEST_SIZE, call_sort);

        printf("num_sorts = %7.0f,  avg. comps = %7.3f,  avg. swaps = %7.3f\n\n",
                num_sorts, num_comps/num_sorts, num_swaps/num_sorts);
} // main
