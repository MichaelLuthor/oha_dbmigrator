#include <stdlib.h>
#include <string.h>
#include "oha_data.h"

boolean oha_data_is_big_ending() {
    union {
        uint16 int_16;
        uint8 int_8;
    } tester;
    tester.int_16 = 1;
    return tester.int_8 == 0;
}

uint8 oha_data_convert_pointer_to_uint8(pointer pointer) {
    uint8 value = 0;
    if ( oha_data_is_big_ending() ) {
        value = ((uint8 *)&pointer)[sizeof(pointer)-1];
    } else {
        value = ((uint8 *)&pointer)[0];
    }
    return value;
}

char * oha_data_malloc_and_copy_string( const char * str ) {
    if ( NULL == str ) {
        return NULL;
    }
    char * new_string = (char *)malloc(strlen(str)+1);
    strcpy(new_string,str);
    return new_string;
}

char * oha_data_string_combine_array( const char ** str_array, const uint32 count, const char * pieces) {
    uint64 target_length = 0;
    for( uint32 i=0; i<count; i++ ) {
        target_length += strlen(str_array[i]);
    }
    if ( NULL != pieces ) {
        target_length += (count-1)*strlen(pieces);
    }

    char * target_str = (char *)malloc(target_length+1);
    target_str[0] = 0;
    for( uint32 i=0; i<count; i++ ) {
        strcat(target_str, str_array[i]);
        if ( NULL!=pieces && i != count-1 ) {
            strcat(target_str, pieces);
        }
    }
    return target_str;
}

void oha_data_pointer_array_free( void *** array, uint32 count ) {
    for ( uint32 i=0; i<count; i++ ) {
        free((*array)[i]);
    }
    free(*array);
}

int oha_data_string_substring_count ( const char * string, const char * substring) {
    int count = 0;
    const char *tmp = string;
    while(NULL != (tmp = strstr(tmp, substring)) ) {
       count++;
       tmp++;
    }
    return count;
}

char * oha_data_string_replace ( const char * source, const char * replace, const char * replace_to ) {
    int replace_count = oha_data_string_substring_count(source,replace);
    uint32 replace_length = strlen(replace);
    uint32 dest_string_length = strlen(source) + strlen(replace_to) * replace_count;
    char * dest_string = (char *)malloc(dest_string_length+1);
    memset(dest_string, 0, dest_string_length+1);

    const char * replace_start_pos = source;
    const char * tmp_string = source;
    for ( int i=0; i<replace_count; i++ ) {
        replace_start_pos = strstr(tmp_string, replace);
        strncat(dest_string, tmp_string, replace_start_pos-tmp_string);
        strcat(dest_string,replace_to);
        tmp_string = replace_start_pos+replace_length;
    }
    strcat(dest_string,tmp_string);
    return dest_string;
}

char * oha_data_string_sub( const char * string, int start, int length) {
    char * dest_string = (char *)malloc(length+1);
    strncpy(dest_string, string+start, length);
    *(dest_string+length) = '\0';
    return dest_string;
}
