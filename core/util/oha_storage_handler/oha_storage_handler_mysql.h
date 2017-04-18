#ifndef __OHA_STORAGE_HANDLER_MYSQL_H__
#define __OHA_STORAGE_HANDLER_MYSQL_H__

#include "oha_storage_handler_mysql.h"
#include "../../library/mysql/mysql.h"

typedef struct _oha_storage_handler_mysql {
    MYSQL * connection;
} oha_storage_handler_mysql;

typedef struct _oha_storage_handler_mysql_query_result {
    MYSQL_RES * result;
} oha_storage_handler_mysql_query_result;

oha_storage_handler_mysql * oha_storage_handler_mysql_create                (const char * host, const char * user, const char * password, const char * dbname, uint32 port);
void *                      oha_storage_handler_mysql_query_table           (void * instance, const char * table, const char * condition);
void                        oha_storage_handler_mysql_query_table_destory   ( void * result );
void                        oha_storage_handler_mysql_destory               (void * instance);
#endif
