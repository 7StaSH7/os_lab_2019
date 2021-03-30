#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  min_max.min = min_max.max = array[begin];

	for (unsigned int i = begin; i < end; i++) {
		if (array[i] > min_max.max) min_max.max = array[i];
		else if (array[i] < min_max.min) min_max.min = array[i];
	}
  
  return min_max;
}
