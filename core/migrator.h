#ifndef __MIGRATOR_H__
#define __MIGRATOR_H__

#include "config.h"
#include "storage.h"

#define MIGRATOR_EXPRESION_PARAM_MAX_COUNT 90

typedef struct _oha_migrator {
    oha_config * config;
    oha_storage * source;
    oha_storage * target;
    oha_config_process * current_process;

    void (*on_process_start)(struct _oha_migrator *, uint64);
    void (*on_process_end)(struct _oha_migrator *);
    void (*on_process_row_insert)(struct _oha_migrator *);
} oha_migrator;

oha_migrator *  oha_migrator_init       (oha_config * config);
void            oha_migrator_process    (oha_migrator * migrator);
void            oha_migrator_destory    (oha_migrator * migrator);
#endif
