#ifndef __MIGRATOR_H__
#define __MIGRATOR_H__

#include "config.h"
#include "storage.h"

typedef struct _oha_migrator {
    oha_config * config;
    oha_storage * source;
    oha_storage * target;
} oha_migrator;

oha_migrator *  oha_migrator_init       (oha_config * config);
void            oha_migrator_process    (oha_migrator * migrator);
void            oha_migrator_destory    (oha_migrator * migrator);
#endif
