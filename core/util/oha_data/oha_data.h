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

uint8 oha_data_convert_pointer_to_uint8(pointer pointer);
#endif
