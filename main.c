#include <stdlib.h>
#include "core/config.h"
#include "core/migrator.h"

int main( int argc, char ** argv ) {
    oha_config * config = oha_config_json_file_load("C:/json.txt");

    oha_migrator * migrator = oha_migrator_init(config);
    oha_migrator_process(migrator);
    oha_migrator_destory(migrator);
    oha_config_destory(config);
    return 0;
}
