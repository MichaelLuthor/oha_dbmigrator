#include <stdio.h>
#include <stdlib.h>
#include "core/config.h"
#include "core/migrator.h"
#include "cli/cli.h"

int main( int argc, char ** argv ) {
    oha_config * config = oha_config_json_file_load("C:/json.txt");
    if ( !config->is_available ) {
        printf("Config Error:%s\n", config->config_message->str);
        return 0;
    }

    oha_migrator * migrator = oha_migrator_init(config);
    cli_migrator_init(migrator);

    oha_migrator_process(migrator);
    oha_migrator_destory(migrator);
    oha_config_destory(config);
    return 0;
}
