#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "util/oha_data/oha_data.h"
#include "library/uthash/uthash.h"

#define STORAGE_TYPE_MYSQL 1

typedef struct _oha_storage_row {
    char * name;
    char * value;
    UT_hash_handle hh;
} oha_storage_row;

typedef struct _oha_storage {
    uint8 type;
    void * instance;

    void *              (*query_table)      (void *,const char *, const char *);
    uint64              (*count)            (void *);
    void                (*query_table_free) (void *);
    void                (*destory)          (void *);
    oha_storage_row *   (*fetch)            (void *);
    boolean             (*insert)           (void *,const char *, oha_storage_row *);
    char *              (*query_get_one)    (void *,const char *);
    char *              (*quote_value)      (void *,const char *);
} oha_storage;

oha_storage *       oha_storage_mysql_create                (const char * host, const char * user, const char * password, const char * dbname, uint32 port);
void *              oha_storage_query_table                 (oha_storage * storage, const char * table, const char * condition );
uint64              oha_storage_query_table_row_count       (oha_storage * storage, void * query_result );
oha_storage_row *   oha_storage_query_table_fetch           (oha_storage * storage, void * result );
void                oha_storage_query_table_destory         (oha_storage * storage, void * query_result );
boolean             oha_storage_insert                      (oha_storage * storage, const char * table, oha_storage_row * row);
void                oha_strage_destory                      (oha_storage * storage );
void                oha_storage_free_row                    (oha_storage_row * data_row );
char *              oha_storage_query_get_one               (oha_storage * storage, const char * query);
char *              oha_storage_quote_value                 (oha_storage * storage, const char * value);
#endif
