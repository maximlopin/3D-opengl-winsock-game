#ifndef INPUT_H
#define INPUT_H

#include "winsock2.h"
#include "stdint.h"

struct Input {
    double cursor_theta = 0.0;
    uint32_t LM_PRESSED = 0;
    uint32_t RM_PRESSED = 0;
    uint32_t NUM1_PRESSED = 0;
    uint32_t NUM2_PRESSED = 0;
    uint32_t NUM3_PRESSED = 0;
    uint32_t NUM4_PRESSED = 0;
    uint32_t NUM5_PRESSED = 0;
    uint32_t NUM6_PRESSED = 0;
    uint32_t NUM7_PRESSED = 0;
    uint32_t NUM8_PRESSED = 0;
    uint32_t NUM9_PRESSED = 0;
    uint32_t NUM0_PRESSED = 0;
};

void hton_input(Input* in, Input* out);

void ntoh_input(Input* in, Input* out);

#endif