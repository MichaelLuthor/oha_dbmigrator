#include <stdlib.h>
#include "config.h"
#include "storage.h"
#include "migrator.h"

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

char * oha_migrator_process_row_handler_item(oha_storage_row * row, uint8 type, char * config) {
    char * value = NULL;
    oha_storage_row * item;

    switch ( type ) {
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_SOURCE_COLUMN :
        HASH_FIND_STR(row, config, item);
        value = item->value;
        break;
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_FIXED_VALUE :
        value = config;
        break;
    case CONFIG_PROCESS_COLUMN_HANDLER_TYPE_FIXED_NULL :
        value = NULL;
        break;
    }
    return value;
}

char * oha_migrator_process_row_handler(oha_storage_row * row, char * name, oha_link * handlers) {
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
    return oha_migrator_process_row_handler_item(row, handler_type, handler_config);
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
        row_item->value = oha_data_malloc_and_copy_string(oha_migrator_process_row_handler(row, row_item->name, column->handlers));
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
