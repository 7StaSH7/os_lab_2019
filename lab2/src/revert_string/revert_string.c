#include "revert_string.h"
#include <stdlib.h>
#include <string.h>

void RevertString(char *str)
{
	int start, end, length;
	char temp;
    char b[1] = { 1 };
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
    
    strcat(str, b);

}

