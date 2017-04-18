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

    return storage;
}

void * oha_storage_query_table( oha_storage * storage, const char * table, const char * condition ) {
    return storage->query_table(storage->instance, table, condition);
}

void oha_storage_query_table_destory(oha_storage * storage, void * query_result ) {
    storage->query_table_free(query_result);
}

void oha_strage_destory( oha_storage * storage ) {
    storage->destory(storage->instance);
    free(storage);
}
//
//char ** oha_storage_fetch(oha_storage * storage, void * query_result) {
//    return storage->fetch(storage->instance, query_result);
//}

//
//void * oha_storage_query(oha_storage * storage, const char * query) {
//    return storage->query(storage->instance, query);
//}
//

//
//boolean oha_storage_insert(oha_storage * storage) {}
//
//void oha_storage_get_error_messagey(oha_storage * storage) {
//
//}


