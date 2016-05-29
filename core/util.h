/**
 * util.h
 * @author Alexander Koch 2016
 * @desc Utility / helper functions.
 */

#ifndef util_h
#define util_h

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
char* strdup(const char* str);
char* strndup(const char* str, size_t n);
#endif

char* concat(char *s1, char *s2);

// djb2 hashing algorithm
unsigned long djb2(unsigned char* str);

// File reading methods
char* readFile(const char* path);
char* replaceExt(char* filename, const char* ext, size_t len);
char* getDirectory(const char* path);

// Memory
void memset64(void* dest, uint64_t value, uintptr_t size);

// Mersenne-Twister
void seed_prng(const uint32_t seed_value);
double prng();

#endif
