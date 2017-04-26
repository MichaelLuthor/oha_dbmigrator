#include <stdlib.h>
#include "storage.h"
#include "util/oha_storage_handler/oha_storage_handler_mysql.h"

oha_storage * oha_storage_mysql_create(const char * host, const char * user, const char * password, const char * dbname, uint32 port) {
    oha_storage * storage = (oha_storage *)malloc(sizeof(oha_storage));
    storage->type = STORAGE_TYPE_MYSQL;
    storage->instance = oha_storage_handler_mysql_create(host,user,password,dbname, port);
    storage->query_table = oha_storage_handler_mysql_query_table;
    storage->query_table_free = oha_storage_handler_mysql_query_table_destory;
    storage->destory = oha_storage_handler_mysql_destory;
    storage->fetch = oha_storage_handler_mysql_query_table_fetch;
    storage->insert = oha_storage_handler_mysql_insert;
    storage->count = oha_storage_handler_mysql_query_table_row_count;
    storage->query_get_one = oha_storage_handler_mysql_query_and_get_one_value;
    storage->quote_value = oha_storage_handler_mysql_quote_value;
    return storage;
}

void * oha_storage_query_table( oha_storage * storage, const char * table, const char * condition ) {
    return storage->query_table(storage->instance, table, condition);
}

uint64 oha_storage_query_table_row_count( oha_storage * storage, void * query_result ) {
    return storage->count(query_result);
}

void oha_storage_query_table_destory(oha_storage * storage, void * query_result ) {
    storage->query_table_free(query_result);
}

void oha_strage_destory( oha_storage * storage ) {
    storage->destory(storage->instance);
    free(storage);
}

oha_storage_row * oha_storage_query_table_fetch(oha_storage * storage, void * result) {
    return storage->fetch(result);
}

boolean oha_storage_insert(oha_storage * storage, const char * table, oha_storage_row * row) {
    return storage->insert(storage->instance, table, row);
}

char * oha_storage_query_get_one(oha_storage * storage, const char * query) {
    return storage->query_get_one(storage->instance, query);
}

char * oha_storage_quote_value(oha_storage * storage, const char * value) {
    return storage->quote_value(storage->instance, value);
}

void oha_storage_free_row ( oha_storage_row * data_row ) {
    oha_storage_row * tmp;
    oha_storage_row * next_tmp;

    for ( tmp=data_row; tmp != NULL;) {
        next_tmp = tmp->hh.next;

        HASH_DEL(data_row, tmp);
        free(tmp->name);
        if ( NULL != tmp->value) {
            free(tmp->value);
        }
        free(tmp);
        tmp = next_tmp;
    }
}
