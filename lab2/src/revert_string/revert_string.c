#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
	int start, end, length;
	char temp;
	length = strlen(str);
	start = 0;
	end = length - 1;
	while (start < end)
	{
		temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++; 
        end--;
	}
}

