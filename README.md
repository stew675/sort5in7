# Sort 5 in 7

Just a solution, written in C, that solves the classic Knuth proposed puzzle of sorting 5 elements in no more than 7 comparisons

5 elements can have a total of 5!, or 120 possible permutations.  Since each comparison is a having of the total "solution" search
space, this means that it requires up to 7 comparisons (2^7 = 128) to fully address all possible inputs.

The mathematical minimum number of comparisons follows the formula `log base2 (120) = ~6.9069` comparisons.  Since comparisons are
discrete operations (there's no such thing as a partial comparison) operating on a discrete number of inputs, the optimal minimum
is actually 6.9333 comparisons on average per input.

The solution as presented provides for the minimum number of comparisons, being 7 comparisons for 112 of the possible inputs, and
6 comparisons for the remaining 8 inputs, resulting in achieving the optimal minimum of 6.9333 comparisons on average, all while
requiring no more than 7 comparisons at any time.
