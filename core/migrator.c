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

void oha_migrator_process(oha_migrator * migrator) {
    oha_link_reset(migrator->config->processes);
    oha_config_process * process = NULL;

    do {
        process = (oha_config_process *)(oha_link_current(migrator->config->processes)->data);
        void * result = oha_storage_query_table(migrator->source, process->source_name->str, process->condition->str);
        void * row = oha_storage_query_table_fetch(migrator->source, result);

        oha_storage_query_table_fetch_destory(migrator->source, row);
        oha_storage_query_table_destory(migrator->source, result);
    } while ( oha_link_next(migrator->config->processes) );
}

void oha_migrator_destory(oha_migrator * migrator) {
    oha_strage_destory(migrator->source);
    oha_strage_destory(migrator->target);
    free(migrator);
}
