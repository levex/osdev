#ifndef __STRING_H_
#define __STRING_H_

#include <stddef.h>
#include <stdint.h>

typedef char* string;

extern size_t strlen(const string str);
extern size_t strcmp(const string str1, const string str2);

#endif
