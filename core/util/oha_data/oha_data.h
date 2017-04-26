#ifndef __OHA_DATA_H__
#define __OHA_DATA_H__

#define OHA_TRUE 1
#define OHA_FALSE 0

#define PLANTFORM_32
#ifdef PLANTFORM_32
typedef unsigned char boolean;
typedef char int8;
typedef unsigned char uint8;
typedef short int int16;
typedef unsigned short int uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef void * pointer;
#endif

uint8   oha_data_convert_pointer_to_uint8   (pointer pointer);
char *  oha_data_malloc_and_copy_string     (const char * str );
char *  oha_data_string_combine_array       (const char ** str_array, const uint32 count, const char * pieces);
char *  oha_data_string_replace             (const char * source, const char * replace, const char * replace_to );
char *  oha_data_string_sub                 (const char * string, int start, int length);
void    oha_data_pointer_array_free         (void *** array, uint32 count );
#endif
