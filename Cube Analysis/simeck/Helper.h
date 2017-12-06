#ifndef HELPER_H__
#define HELPER_H__

#include <Windows.h>
#include <stdint.h>

char** str_split(char* a_str, const char a_delim);
void ErrorHandler(LPTSTR lpszFunction);
void array_rnd_fill(int *CUBE,int SIZE,int start,int end);
void index_rnd_fill(int *CUBE,int index,int SIZE,int start,int end);
void long_to_bits64(uint64_t x);
void long_to_bits16(uint16_t x);
void long_to_bits(uint64_t x);
uint64_t rand64();
uint32_t rand32();
uint16_t rand16();
int rand_int(int a, int b);
int createLogFile(const char* path);

#endif