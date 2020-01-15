#ifndef INPUT_H
#define INPUT_H

#include "winsock2.h"
#include "stdint.h"

struct Input {
    double cursor_theta;
    uint32_t LM_PRESSED;
    uint32_t RM_PRESSED;
};

void hton_input(Input* in, Input* out);

void ntoh_input(Input* in, Input* out);

#endif