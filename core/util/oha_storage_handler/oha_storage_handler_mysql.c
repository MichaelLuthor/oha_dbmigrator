
#include <stdio.h>
#include <stdlib.h>
#include "../../library/mysql/mysql.h"
#include "../oha_data/oha_data.h"
#include "oha_storage_handler_mysql.h"

oha_storage_handler_mysql * oha_storage_handler_mysql_create(const char * host, const char * user, const char * password, const char * dbname, uint32 port) {
    oha_storage_handler_mysql * mysql = NULL;
    mysql = (oha_storage_handler_mysql *)malloc(sizeof(oha_storage_handler_mysql));

    char *unix_socket = NULL;
    unsigned long client_flag = 0;

    MYSQL * connection = mysql_init(NULL);
    mysql_real_connect(connection, host, user, password, dbname, port, unix_socket, client_flag);

    mysql->connection = connection;
    return mysql;
}

void * oha_storage_handler_mysql_query_table ( void * instance, const char * table, const char * condition) {
    oha_storage_handler_mysql * mysql = (oha_storage_handler_mysql *)instance;

    char * query_template = "SELECT * FROM `%s` WHERE %s";
    uint32 query_length = strlen(table) + strlen(query_template) + 1;
    if ( NULL != condition ) {
        query_length += strlen(condition);
    }

    char * query = (char *)malloc(query_length);
    sprintf(query, query_template, table, (NULL==condition)?"1":condition);

    int result_code = mysql_query(mysql->connection, query);
    free(query);

    if ( 0 != result_code ) {
        return NULL;
    }
    oha_storage_handler_mysql_query_result * result = NULL;
    result = (oha_storage_handler_mysql_query_result *)malloc(sizeof(oha_storage_handler_mysql_query_result));

    result->result = mysql_store_result(mysql->connection);
    return (void *)result;
}

void * oha_storage_handler_mysql_query_table_fetch ( void * result ) {
    return NULL;
}

void oha_storage_handler_mysql_query_table_destory ( void * result ) {
    MYSQL_RES * mysql_result = (MYSQL_RES * )result;
    mysql_free_result(mysql_result);
}

void oha_storage_handler_mysql_destory(void * instance) {
    oha_storage_handler_mysql * mysql = (oha_storage_handler_mysql *)instance;
    mysql_close(mysql->connection);
    free(instance);
}







char * oha_storage_handler_get_error_message(void * instance) {
    return NULL;
}

boolean oha_storage_handler_mysql_insert(void * instance, const char ** row) {
    return OHA_TRUE;
}
