#include <stdio.h>
#include <stdlib.h>
#include "../../storage.h"
#include "../../library/mysql/mysql.h"
#include "../../library/uthash/uthash.h"
#include "../oha_data/oha_data.h"
#include "../oha_string/oha_string.h"
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
    oha_storage_handler_mysql_query_result * mysql_result = (oha_storage_handler_mysql_query_result *)result;
    oha_storage_row * row_head = NULL;
    oha_storage_row * row_item = NULL;

    MYSQL_FIELD * fields = mysql_fetch_fields(mysql_result->result);
    MYSQL_ROW data_row = mysql_fetch_row(mysql_result->result);
    unsigned int num_fields = mysql_num_fields(mysql_result->result);

    for(unsigned int i = 0; i < num_fields; i++) {
        row_item = (oha_storage_row *)malloc(sizeof(oha_storage_row));
        row_item->name = oha_data_malloc_and_copy_string(fields[i].name);
        row_item->value = oha_data_malloc_and_copy_string(data_row[i]);
        HASH_ADD_STR(row_head, name, row_item);
    }
    return row_head;
}

void oha_storage_handler_mysql_query_table_fetch_destory ( void * row ) {
    oha_storage_row * data_row = (oha_storage_row *)row;
    oha_storage_row * tmp;
    oha_storage_row * next_tmp;

    for ( tmp=data_row; tmp != NULL;) {
        HASH_DEL(data_row, tmp);
        next_tmp = tmp->hh.next;
        free(tmp->name);
        if ( NULL != tmp->value) {
            free(tmp->value);
        }
        free(tmp);
        tmp = next_tmp;
    }
}

void oha_storage_handler_mysql_query_table_destory ( void * result ) {
    oha_storage_handler_mysql_query_result * mysql_result = NULL;
    mysql_result = (oha_storage_handler_mysql_query_result *)result;
    mysql_free_result(mysql_result->result);
    free(mysql_result);
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
