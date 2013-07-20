#ifndef __UTILS_H
#define __UTILS_H

/* Statically compute the base-2 logarithm of a number up to 32. */
#define LOG2(x) ((x) > 32 ? 6 : ((x) > 16 ? 5 : ((x) > 8 ? 4 : ((x) > 4 ? 3 : 2))))

/* Calculate the number of elements in an array. */
#define array_length(arr) (sizeof(arr) / sizeof(arr[0]))

#endif