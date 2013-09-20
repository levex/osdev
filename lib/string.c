#include "../include/string.h"
#include <stddef.h>
#include <stdint.h>

size_t strlen(string str)
{
	size_t i = 0;
	while(*str != 0) i++;
	return i;
}

