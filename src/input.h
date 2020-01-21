#ifndef INPUT_H
#define INPUT_H

#include "winsock2.h"
#include "stdint.h"

struct Input {
    double cursor_theta = 0.0;
    uint32_t LM_PRESSED = 0;
    uint32_t RM_PRESSED = 0;
};

void hton_input(Input* in, Input* out);

void ntoh_input(Input* in, Input* out);

#endif