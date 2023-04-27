#pragma once

#include <stdint.h>

#define PATH_NO_VALUE 0xFFFF

typedef struct
{
  char* ptr;
  uint16_t size;
} mut_cstring;

/*
	Description of the given path.
	
	If the size == 0xFFFF, then the given char* does not exist.
*/
typedef struct FILE_PATH_DESC
{
	mut_cstring absolute_dir; // ex. C:/projects
	mut_cstring file_name; // ex. pathsplit	
	mut_cstring ext; // ex. `exe`, `h`, `ass`
} FILE_PATH;

/*
	Splits the path and allocates all arrays in `desc` with proper data.
	
	If `file_path_size` == 0 then it will determine size itself.
*/
void split_path(char* file_path, const uint16_t file_path_size, FILE_PATH* desc);

/*
	Frees all char arrays inside of the `desc`.
	It won't free the `desc` itself.
*/
void free_path(FILE_PATH* desc);

/*

*/
void change_slashes(char* str, uint16_t size);

/*
	Creates a null-terminated string from FILE_PATH content and returns a pointer to it.
*/
char* create_string(FILE_PATH* desc);

/*
	Adds a string at the end of input with new size.
	Expects for strings to be null terminated.
*/
void add_to_string(mut_cstring* dest, mut_cstring* suffix);