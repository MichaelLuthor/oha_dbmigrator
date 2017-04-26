#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "storage.h"
#include "migrator.h"
#include "library/pcre/pcre.h"

oha_migrator * oha_migrator_init(oha_config * config) {
    oha_migrator * migrator = (oha_migrator *)malloc(sizeof(oha_migrator));
    migrator->config = config;

    oha_config_storage source = config->source;
    switch ( config->source.type ) {
    case STORAGE_TYPE_MYSQL :
        migrator->source = oha_storage_mysql_create(source.host->str,source.user->str,source.password->str,source.dbname->str,source.port);
        break;
    }

    oha_config_storage target = config->target;
    switch ( config->target.type ) {
    case STORAGE_TYPE_MYSQL :
        migrator->target = oha_storage_mysql_create(target.host->str,target.user->str,target.password->str,target.dbname->str,target.port);
        break;
    }

    return migrator;
}

char * oha_migrator_process_replace_param_placeholder(oha_migrator * migrator, const char * expression, oha_storage_row * row) {
    const char *error;
    int  erroffset;
    pcre * regex = pcre_compile("(\\{(.*?)\\})",0,&error,&erroffset,NULL);
    if (regex == NULL) {
        return NULL;
    }

    char * dest_expression = expression;
    char * tmp_dest_expression = NULL;

    int  match_count = 0;
    unsigned int match_offset = 0;
    oha_storage_row * place_holder_value_item;
    unsigned int expression_len = strlen(expression);
    int *ovector = (int *)malloc(sizeof(int)*MIGRATOR_EXPRESION_PARAM_MAX_COUNT);

    memset(ovector, 0, sizeof(int)*MIGRATOR_EXPRESION_PARAM_MAX_COUNT);
    do {
        if ( match_offset >= expression_len ) {
            break;
        }
        match_count = pcre_exec(regex,NULL,expression,expression_len,match_offset,0,ovector,MIGRATOR_EXPRESION_PARAM_MAX_COUNT);
        if ( match_count < 0 ) {
            break;
        }

        char * place_holder = oha_data_string_sub(expression, ovector[2], ovector[3]-ovector[2]);
        char * param = oha_data_string_sub(expression, ovector[4], ovector[5]-ovector[4]);

        HASH_FIND_STR(row, param, place_holder_value_item);
        char * place_holder_value = oha_data_malloc_and_copy_string(place_holder_value_item->value);
        char * quoted_place_holder_value = oha_storage_quote_value(migrator->source, place_holder_value);

        tmp_dest_expression = oha_data_string_replace(dest_expression, place_holder, quoted_place_holder_value);
        if ( dest_expression != expression ) {
            free(dest_expression);
        }
        dest_expression = tmp_dest_expression;

        free(quoted_place_holder_value);
        free(place_holder_value);
        free(place_holder);
        free(param);
        match_offset = ovector[1];
    } while ( 1 );

    pcre_free(regex);
    free(ovector);
    return dest_expression;
}

char * oha_migrator_process_row_handler_item(oha_migrator * migrator, oha_storage_row * row, uint8 type, char * config) {
    char * value = NULL;
    oha_storage_row * item;

    char * query = NULL;
    switch ( type ) {
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_SOURCE_COLUMN :
        HASH_FIND_STR(row, config, item);
        value = oha_data_malloc_and_copy_string(item->value);
        break;
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_FIXED_VALUE :
        value = oha_data_malloc_and_copy_string(config);
        break;
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_FIXED_NULL :
        value = NULL;
        break;
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_QUERY :
        query = oha_migrator_process_replace_param_placeholder(migrator, config, row);
        if ( NULL != query ) {
            value = oha_storage_query_get_one(migrator->source, query);
        }
        free(query);
        break;
    }
    return value;
}

char * oha_migrator_process_row_handler(oha_migrator * migrator, oha_storage_row * row, char * name, oha_link * handlers) {
    uint8 handler_type = 0;
    char * handler_config = NULL;

    oha_link_reset(handlers);
    do {
        oha_config_process_column_handler * handler = NULL;
        handler = (oha_config_process_column_handler *)(oha_link_current(handlers)->data);
        switch ( handler->condition_type ) {
        case CONFIG_PROCESS_COLUMN_CONDITION_TYPE_NULL :
            handler_type = handler->handle_type;
            handler_config = handler->handle_config->str;
            break;
        default : break;
        }

        if ( 0 != handler_type ) {
            break;
        }
    } while ( oha_link_next(handlers) );
    return oha_migrator_process_row_handler_item(migrator, row, handler_type, handler_config);
}

oha_storage_row * oha_migrator_process_row(oha_migrator * migrator, oha_config_process * process, oha_storage_row * row) {
    oha_storage_row * row_head = NULL;
    oha_storage_row * row_item = NULL;

    oha_link * columns = process->columns;
    oha_link_reset(columns);
    do {
        oha_config_process_colum * column = NULL;
        column = (oha_config_process_colum *)(oha_link_current(columns)->data);

        row_item = (oha_storage_row *)malloc(sizeof(oha_storage_row));
        row_item->name = oha_data_malloc_and_copy_string(column->target_column_name->str);
        char * value = oha_migrator_process_row_handler(migrator, row, row_item->name, column->handlers);
        row_item->value = oha_data_malloc_and_copy_string(value);
        if ( NULL != value ) {
            free(value);
        }
        HASH_ADD_STR(row_head, name, row_item);
    } while ( oha_link_next(columns) );

    return row_head;
}

void oha_migrator_process(oha_migrator * migrator) {
    oha_link_reset(migrator->config->processes);
    oha_config_process * process = NULL;

    do {
        process = (oha_config_process *)(oha_link_current(migrator->config->processes)->data);
        void * result = oha_storage_query_table(migrator->source, process->source_name->str, process->condition->str);

        migrator->current_process = process;
        migrator->on_process_start(migrator, oha_storage_query_table_row_count(migrator->source, result));
        do {
            oha_storage_row * row = oha_storage_query_table_fetch(migrator->source, result);
            if ( NULL == row ) {
                break;
            }
            oha_storage_row * target_row = oha_migrator_process_row(migrator,process, row);
            oha_storage_insert(migrator->target, process->targe_name->str, target_row);

            migrator->on_process_row_insert(migrator);

            oha_storage_free_row(row);
            oha_storage_free_row(target_row);
        } while(1);
        migrator->on_process_end(migrator);

        oha_storage_query_table_destory(migrator->source, result);
    } while ( oha_link_next(migrator->config->processes) );
}

void oha_migrator_destory(oha_migrator * migrator) {
    oha_strage_destory(migrator->source);
    oha_strage_destory(migrator->target);
    free(migrator);
}
