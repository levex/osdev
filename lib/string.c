/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/string.h"
#include <stddef.h>
#include <stdint.h>

size_t strlen(const string str)
{
	size_t i = 0;
	while(str[i] != 0) i++;
	return i;
}

size_t strcmp(string str1, string str2)
{
	size_t res=0;
	while (!(res = *(unsigned char*)str1 - *(unsigned char*)str2) && *str2)
		++str1, ++str2;

	if (res < 0)
		res = -1;
	if (res > 0)
		res = 1;

	return res;
}

