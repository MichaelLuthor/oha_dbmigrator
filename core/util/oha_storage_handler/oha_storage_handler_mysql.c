#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

uint64 oha_storage_handler_mysql_query_table_row_count(void * result) {
    oha_storage_handler_mysql_query_result * mysql_result = (oha_storage_handler_mysql_query_result *)result;
    return mysql_num_rows(mysql_result->result);
}

oha_storage_row * oha_storage_handler_mysql_query_table_fetch ( void * result ) {
    oha_storage_handler_mysql_query_result * mysql_result = (oha_storage_handler_mysql_query_result *)result;
    oha_storage_row * row_head = NULL;
    oha_storage_row * row_item = NULL;

    MYSQL_ROW data_row = mysql_fetch_row(mysql_result->result);
    if ( NULL == data_row ) {
        return NULL;
    }

    MYSQL_FIELD * fields = mysql_fetch_fields(mysql_result->result);
    unsigned int num_fields = mysql_num_fields(mysql_result->result);

    for(unsigned int i = 0; i < num_fields; i++) {
        row_item = (oha_storage_row *)malloc(sizeof(oha_storage_row));
        row_item->name = oha_data_malloc_and_copy_string(fields[i].name);
        row_item->value = oha_data_malloc_and_copy_string(data_row[i]);
        HASH_ADD_STR(row_head, name, row_item);
    }
    return row_head;
}

void oha_storage_handler_mysql_query_table_destory ( void * result ) {
    oha_storage_handler_mysql_query_result * mysql_result = NULL;
    mysql_result = (oha_storage_handler_mysql_query_result *)result;
    mysql_free_result(mysql_result->result);
    free(mysql_result);
}

boolean oha_storage_handler_mysql_insert(void * instance, const char * table, oha_storage_row * row) {
    oha_storage_handler_mysql * mysql = (oha_storage_handler_mysql *)instance;

    uint32 column_count = HASH_COUNT(row);
    char ** columns = (char **)malloc(sizeof(char *) * column_count);
    char ** values = (char **)malloc(sizeof(char *) * column_count);

    oha_storage_row * tmp;
    uint32 index = 0;
    uint32 column_value_length = 0;
    for (tmp=row; tmp!=NULL; tmp=tmp->hh.next) {
        columns[index] = (char *)malloc(strlen(tmp->name)*2+1);
        sprintf(columns[index], "`%s`", tmp->name);

        if ( NULL == tmp->value ) {
            values[index] = (char *)malloc(5);
            sprintf(values[index], "NULL");
        } else {
            column_value_length = strlen(tmp->value);
            values[index] = (char *)malloc(column_value_length*3+1);
            char * tmp_value = (char *)malloc(column_value_length*3+1);
            mysql_real_escape_string(mysql->connection, tmp_value, tmp->value, column_value_length);
            sprintf(values[index], "\"%s\"", tmp_value);
            free(tmp_value);
        }

        index++;
    }

    char * column_string = oha_data_string_combine_array(columns, column_count, ",");
    char * value_string = oha_data_string_combine_array(values, column_count, ",");
    oha_data_pointer_array_free(&columns,column_count);
    oha_data_pointer_array_free(&values,column_count);

    char * query_template = "INSERT INTO `%s` (%s) VALUES (%s)";
    char * query = (char *)malloc(strlen(query_template) + strlen(column_string) + strlen(value_string) + strlen(table) + 100);
    sprintf(query, query_template, table, column_string, value_string);
    free(column_string);
    free(value_string);

    int query_result = mysql_query(mysql->connection, query);
    free(query);
    return 0==query_result ? OHA_TRUE : OHA_FALSE;
}

void oha_storage_handler_mysql_destory(void * instance) {
    oha_storage_handler_mysql * mysql = (oha_storage_handler_mysql *)instance;
    mysql_close(mysql->connection);
    free(instance);
}
