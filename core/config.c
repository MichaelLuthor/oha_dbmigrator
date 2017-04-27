#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "library/jansson/jansson.h"
#include "util/oha_string/oha_string.h"
#include "util/oha_link/oha_link.h"
#include "util/oha_data/oha_data.h"

boolean oha_config_error(oha_config * config, char * message) {
    config->is_available = OHA_FALSE;
    if ( NULL == config->config_message ) {
        config->config_message = oha_string_create(message);
    } else {
        oha_string_set(config->config_message, message);
    }
    return OHA_FALSE;
}

oha_config_process_column_handler * oha_config_column_handler_create(uint8 type, char * config,uint8 condition_type, const char * condition_config) {
    oha_config_process_column_handler * handler = NULL;
    handler = (oha_config_process_column_handler *)malloc(sizeof(oha_config_process_column_handler));
    handler->handle_type = type;
    handler->handle_config = oha_string_create(config);
    handler->condition_type = condition_type;
    handler->condition_config = oha_string_create(condition_config);

    return handler;
}

void oha_config_column_handler_destory( oha_config_process_column_handler * handler ) {
    oha_string_destory(handler->handle_config);
    oha_string_destory(handler->condition_config);
    free(handler);
}

oha_config_process_colum * oha_config_column_create(const char * name, uint8 target_data_type, oha_link * handlers) {
    oha_config_process_colum * column = NULL;
    column = (oha_config_process_colum *)malloc(sizeof(oha_config_process_colum));

    column->target_column_name = oha_string_create(name);
    column->target_data_type = target_data_type;
    column->handlers = handlers;
    return column;
}

void oha_config_column_destory( oha_config_process_colum * column ) {
    oha_link_reset(column->handlers);
    do {
        oha_config_process_column_handler * handler = NULL;
        handler = (oha_config_process_column_handler *)(oha_link_current(column->handlers)->data);
        oha_config_column_handler_destory(handler);
    } while ( OHA_TRUE == oha_link_next(column->handlers) );
    oha_link_desctory(column->handlers);

    oha_string_destory(column->target_column_name);
    free(column);
}

oha_config_process * oha_config_process_create(const char * name, const char * source, const char * target, oha_link * columns, const char * condition) {
    oha_config_process * process = (oha_config_process *)malloc(sizeof(oha_config_process));
    process->name = oha_string_create(name);
    process->source_name = oha_string_create(source);
    process->targe_name = oha_string_create(target);
    process->condition =  oha_string_create(condition);
    process->columns = columns;

    return process;
}

void oha_config_process_destory( oha_config_process * process ) {
    oha_link_reset(process->columns);
    do {
        oha_config_process_colum * column = NULL;
        column = (oha_config_process_colum *)(oha_link_current(process->columns)->data);
        oha_config_column_destory(column);
    } while ( OHA_TRUE == oha_link_next(process->columns) );
    oha_link_desctory(process->columns);

    oha_string_destory(process->name);
    oha_string_destory(process->source_name);
    oha_string_destory(process->targe_name);
    if ( NULL != process->condition ) {
        oha_string_destory(process->condition);
    }
    free(process);
}

void oha_config_destory( oha_config * config ) {
    oha_link_reset(config->processes);
    do {
        oha_config_process * process = NULL;
        process = (oha_config_process *)(oha_link_current(config->processes)->data);
        oha_config_process_destory(process);
    } while ( OHA_TRUE == oha_link_next(config->processes) );
    oha_link_desctory(config->processes);

    if ( NULL != config->config_message ) {
        oha_string_destory(config->config_message);
    }
    free(config);
}

void * oha_config_read_json_by_path(json_t * json, const char * path ) {
    char * ori_path = (char *)malloc(strlen(path)+1);
    strcpy(ori_path, path);

    char * path_item = strtok(ori_path, ".");
    json_t * json_item = json;
    while ( NULL != path_item && json_is_object(json_item) ) {
        json_item = json_object_get(json_item, path_item);
        path_item = strtok(NULL, ".");
    }

    void * value = NULL;
    if ( json_is_object(json_item) ) {
        value = (void *)json_item;
    } else if ( json_is_string(json_item) ) {
        value = (void *)json_string_value(json_item);
    } else if ( json_is_integer(json_item) ) {
        value = (void *)json_integer_value(json_item);
    } else if (json_is_array(json_item) ) {
        value = (void *)json_item;
    } else {
        value = NULL;
    }
    free(ori_path);
    return value;
}

oha_link * oha_config_json_file_load_column_handlers(oha_config * config, json_t * column_config) {
    oha_link * handler_link = oha_link_create();

    /** setup column handlers */
    json_t * handlers = (json_t *)oha_config_read_json_by_path(column_config, "handlers");
    size_t handler_length = json_array_size(handlers);
    for ( size_t i=0; i<handler_length; i++ ) {
        json_t * handler = json_array_get(handlers, i);

        pointer handle_type = (pointer)oha_config_read_json_by_path(handler, "handle.type");
        char * handle_config = (char *)oha_config_read_json_by_path(handler, "handle.config");
        pointer condition_type = (pointer)oha_config_read_json_by_path(handler, "condition.type");
        char * condition_config = (char *)oha_config_read_json_by_path(handler, "condition.config");
        if ( NULL==handle_type || NULL==handle_config || NULL==condition_type || NULL==condition_config ) {
            oha_config_error(config, "Column handler error");
            break;
        }

        uint8 u8_handler_type = oha_data_convert_pointer_to_uint8(handle_type);
        uint8 u8_config_type = oha_data_convert_pointer_to_uint8(condition_type);
        oha_config_process_column_handler * column_handler = NULL;
        column_handler = oha_config_column_handler_create(u8_handler_type, handle_config,u8_config_type,condition_config);
        oha_link_append(handler_link, column_handler);
    }
    return handler_link;
}

oha_link * oha_config_json_file_load_process_column_config (oha_config * config, json_t * process_config ) {
    oha_link * columns = oha_link_create();
    json_t * config_columns = (json_t *)oha_config_read_json_by_path(process_config, "colums");
    if ( NULL == config_columns ) {
       oha_config_error(config, "Invalid colum config.");
       return NULL;
   }

    const char * column_name;
    json_t * column_config;

    void *iter = json_object_iter(config_columns);
    while(config->is_available && iter) {
        column_name = json_object_iter_key(iter);
        column_config = json_object_iter_value(iter);

        pointer target_data_type = oha_config_read_json_by_path(column_config, "target_data_type");
        uint8 u8_target_data_type = oha_data_convert_pointer_to_uint8(target_data_type);

        oha_link * handlers = oha_config_json_file_load_column_handlers(config, column_config);
        oha_config_process_colum * column = oha_config_column_create(column_name, u8_target_data_type, handlers);
        oha_link_append(columns, column);

        iter = json_object_iter_next(config_columns, iter);
    }
    return columns;
}

oha_link * oha_config_json_file_load_process_config(oha_config * config, json_t * config_json) {
    json_t * config_processs = (json_t *)oha_config_read_json_by_path(config_json, "process");
    if ( NULL == config_processs ) {
        oha_config_error(config, "Invalid process config.");
        return NULL;
    }

    oha_link * processes = oha_link_create();
    const char * process_name;
    json_t * process_config;
    void * iter = json_object_iter(config_processs);
    while(config->is_available && iter) {
        process_name = json_object_iter_key(iter);
        process_config = json_object_iter_value(iter);

        char * source = (char *)oha_config_read_json_by_path(process_config,"source");
        char * targe = (char *)oha_config_read_json_by_path(process_config,"target");
        char * condition = (char *)oha_config_read_json_by_path(process_config,"condition");
        oha_link * columns = oha_config_json_file_load_process_column_config(config, process_config);
        oha_config_process * process = oha_config_process_create(process_name,source,targe,columns,condition);
        oha_link_append(processes, process);

        iter = json_object_iter_next(config_processs, iter);
    }
    return processes;
}

oha_config * oha_config_json_file_load ( const char * file_path ) {
    oha_config * config = (oha_config *)malloc(sizeof(oha_config));
    config->is_available = OHA_TRUE;
    config->config_message = NULL;

    json_t * file_json;
    json_error_t error;
    file_json = json_load_file(file_path, 0, &error);
    if(!file_json) {
        oha_config_error(config, "Invalid config file.");
    }

    if ( config->is_available ) {
        config->source.type = oha_data_convert_pointer_to_uint8(oha_config_read_json_by_path(file_json, "source.type"));
        switch ( config->source.type ) {
        case STORAGE_TYPE_MYSQL :
            config->source.host = oha_string_create((char *)oha_config_read_json_by_path(file_json, "source.host"));
            config->source.user = oha_string_create((char *)oha_config_read_json_by_path(file_json, "source.user"));
            config->source.password = oha_string_create((char *)oha_config_read_json_by_path(file_json, "source.password"));
            config->source.dbname = oha_string_create((char *)oha_config_read_json_by_path(file_json, "source.dbname"));
            config->source.port = 3306;
            break;
        default :
            oha_config_error(config, "Invalid source config.");
            break;
        }
    }

    if ( config->is_available ) {
        config->target.type = oha_data_convert_pointer_to_uint8(oha_config_read_json_by_path(file_json, "target.type"));
        switch ( config->target.type ) {
        case STORAGE_TYPE_MYSQL :
            config->target.host = oha_string_create((char *)oha_config_read_json_by_path(file_json, "target.host"));
            config->target.user = oha_string_create((char *)oha_config_read_json_by_path(file_json, "target.user"));
            config->target.password = oha_string_create((char *)oha_config_read_json_by_path(file_json, "target.password"));
            config->target.dbname = oha_string_create((char *)oha_config_read_json_by_path(file_json, "target.dbname"));
            config->target.port = 3306;
            break;
        default :
            oha_config_error(config, "Invalid target config.");
            break;
        }
    }

    config->php_script = oha_string_create((char *)oha_config_read_json_by_path(file_json, "php_script"));
    if ( config->is_available ) {
        config->processes = oha_config_json_file_load_process_config(config, file_json);
    }

    free(file_json);
    return config;
}
