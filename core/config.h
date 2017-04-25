#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "util/oha_data/oha_data.h"
#include "util/oha_string/oha_string.h"
#include "util/oha_link/oha_link.h"

#define STORAGE_TYPE_MYSQL      1
#define STORAGE_TYPE_SQLITE     2
#define STORAGE_TYPE_MSSQL      3
#define STORAGE_TYPE_CSV        4
#define STORAGE_TYPE_EXCEL      5
#define STORAGE_TYPE_FIREBIRD   6
#define STORAGE_TYPE_POSTGRESQL 7

#define CONFIG_PROCESS_COLUMN_CONDITION_TYPE_NULL 1 /* no condition, the always true. */

#define CONFIG_PROCESS_COLUMN_HANDLER_TYPE_SOURCE_COLUMN 1
#define CONFIG_PROCESS_COLUMN_HANDLER_TYPE_FIXED_VALUE 2


#define STORAGE_DATA_TYPE_NUMBER    1
#define STORAGE_DATA_TYPE_oha_string    2
#define STORAGE_DATA_TYPE_DATETIME  3
#define STORAGE_DATA_TYPE_DATE      4
#define STORAGE_DATA_TYPE_TIME      5

#define COLUMN_HANDLER_TYPE_CALL 1
#define COLUMN_HANDLER_TYPE_EXPR 2


typedef struct _oha_config_process_column_handler {
    uint8 handle_type;
    oha_string * handle_config;
    uint8 condition_type;
    oha_string * condition_config;
} oha_config_process_column_handler;

typedef struct _oha_config_process_column {
    oha_string * target_column_name;
    uint8 target_data_type;
    oha_link * handlers;
} oha_config_process_colum;

typedef struct _oha_config_process {
    oha_string * name;
    oha_string * source_name;
    oha_string * targe_name;
    oha_string * condition;
    oha_link * columns;
} oha_config_process;

typedef struct _oha_config_storage {
    uint8 type;
    oha_string * host;
    oha_string * user;
    oha_string * password;
    oha_string * dbname;
    oha_string * file_path;
    uint32 port;
} oha_config_storage;

typedef struct _oha_config {
    boolean is_available;
    oha_string * config_message;
    oha_config_storage source;
    oha_config_storage target;
    oha_link * processes;
} oha_config;

oha_config_process_column_handler * oha_config_column_handler_create    (uint8 type, char * config,uint8 condition_type, const char * condition_config);
void                                oha_config_column_handler_destory   ( oha_config_process_column_handler * handler );
oha_config_process_colum *          oha_config_column_create            (const char * name, uint8 target_data_type, oha_link * handlers);
void                                oha_config_column_destory           ( oha_config_process_colum * column );
oha_config_process *                oha_config_process_create           (const char * name, const char *  source, const char *  target, oha_link * columns, const char * condition);
void                                oha_config_process_destory          ( oha_config_process * process );
void                                oha_config_destory                  ( oha_config * config );
oha_config *                        oha_config_json_file_load           ( const char * file_path );

#endif
