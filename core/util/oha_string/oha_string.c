#include <stdlib.h>
#include <string.h>
#include "../oha_data/oha_data.h"
#include "oha_string.h"

oha_string * oha_string_create(const char * str) {
    oha_string * string = (oha_string *)malloc(sizeof(oha_string));
    string->len = 0;
    string->str = NULL;
    string->size = 0;
    
    if ( NULL != str ) {
        string->len = strlen(str);
        string->size = string->len * 2;
        string->str = (char *)malloc(string->size);
        strcpy(string->str, str);
    }
    return string;
}

void oha_string_set( oha_string * string, const char * str ) {
    uint32 new_length = strlen(str);
    if ( new_length > string->size ) {
        string->size = new_length * 2;
        free(string->str);
        string->str = (char *)malloc(string->size);
    }
    strcpy(string->str, str);
    string->len = new_length;
}

void oha_string_destory(oha_string * str) {
    if ( NULL != str->str ) {
        free(str->str);
    } 
    free(str);
}
