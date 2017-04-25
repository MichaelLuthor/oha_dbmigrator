#include <stdio.h>
#include "cli.h"
#include "../core/migrator.h"

uint64 cli_migrator_process_row_counter = 0;
uint64 cli_migrator_process_row_count = 0;

void cli_migrator_process_row(oha_migrator * migrator) {
    cli_migrator_process_row_counter ++;
    oha_config_process * process = migrator->current_process;
    printf("Process [%s:%s=>%s] %lld/%lld\n",
        process->name->str, process->source_name->str, process->targe_name->str,
        cli_migrator_process_row_counter,
        cli_migrator_process_row_count);
}

void cli_migrator_process_start(oha_migrator * migrator, uint64 row_count) {
    cli_migrator_process_row_count = row_count;
    cli_migrator_process_row_counter = 0;
    oha_config_process * process = migrator->current_process;
    printf("Process [%s:%s=>%s] Started, %lld rows found.\n",
        process->name->str, process->source_name->str, process->targe_name->str, row_count);
}

void cli_migrator_process_end(oha_migrator * migrator) {
    oha_config_process * process = migrator->current_process;
    printf("Process [%s:%s=>%s] Done\n",
        process->name->str, process->source_name->str, process->targe_name->str);
}

void cli_migrator_init_start(oha_migrator * migrator) {
    oha_config_storage source = migrator->config->source;
    switch ( source.type ) {
    case STORAGE_TYPE_MYSQL :
        printf("Source : %s@%s:%d (User:%s Pass:%s)\n",
            source.dbname->str, source.host->str, source.port, source.user->str, source.password->str);
        break;
    default :
        break;
    }

    oha_config_storage target = migrator->config->target;
    switch ( target.type ) {
    case STORAGE_TYPE_MYSQL :
        printf("Target : %s@%s:%d (User:%s Pass:%s)\n",
            target.dbname->str, target.host->str, target.port, target.user->str, target.password->str);
        break;
    default :
        break;
    }
}

void cli_migrator_init( oha_migrator * migrator ) {
    setbuf(stdout,NULL);
    cli_migrator_init_start(migrator);

    migrator->on_process_start = cli_migrator_process_start;
    migrator->on_process_end = cli_migrator_process_end;
    migrator->on_process_row_insert = cli_migrator_process_row;
}
