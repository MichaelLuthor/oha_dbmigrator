#include "oha_data.h"

boolean oha_data_is_big_ending() {
    union {
        uint16 int_16;
        uint8 int_8;
    } tester;
    tester.int_16 = 1;
    return tester.int_8 == 0;
}

uint8 oha_data_convert_pointer_to_uint8(pointer pointer) {
    uint8 value = 0;
    if ( oha_data_is_big_ending() ) {
        value = ((uint8 *)&pointer)[sizeof(pointer)-1];
    } else {
        value = ((uint8 *)&pointer)[0];
    }
    return value;
}
