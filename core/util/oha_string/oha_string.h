#ifndef __OHA_STRING_H__
#define __OHA_STRING_H__

typedef struct _oha_string {
    char * str;
    unsigned long len;
    unsigned long size;
} oha_string;

oha_string *    oha_string_create   (const char * str);
void            oha_string_destory  (oha_string * str);
void            oha_string_set      (oha_string * string, const char * str );
#endif
