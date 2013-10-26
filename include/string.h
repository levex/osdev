#ifndef __STRING_H_
#define __STRING_H_

#include <stddef.h>
#include <stdint.h>

typedef char* string;

extern size_t strlen(const string str);
extern size_t strcmp(const string str1, const string str2);

extern size_t strcrl(string str, const char what, const char with);
extern size_t str_begins_with(const string str, const string with);
extern size_t str_backspace(string str, char c);
extern size_t strcount(string str, char c);
extern size_t strsplit(string str, char delim);

#endif
