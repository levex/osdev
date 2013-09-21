#include "../include/string.h"
#include <stddef.h>
#include <stdint.h>

size_t strlen(const string str)
{
	size_t i = 0;
	while(str[i] != 0) i++;
	return i;
}

