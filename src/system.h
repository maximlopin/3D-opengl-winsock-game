#ifndef SYSTEM_H
#define SYSTEM_H

#include "stdint.h"
#include "winsock2.h"
#include "cglm/cglm.h"
#include "eclass.h"

/*
    Sync entity packet structure

    -------------------------------
    uint8_t |  N (number of entities)
    -------------------------------
    uint8_t |
       .    |
       .    |  Each of N uint8_t are
       .    |  members of EClass struct.
       N    |
    -------------------------------
      BYTES | Some data, each entity
        .   | from above header is
        .   | responsible for consuming
        .   | it.
        M   |
    -------------------------------
*/

static const uint16_t MAX_BUF_SIZE = ((2 << 15) - 1);
static const uint16_t MAX_PACKET_SIZE = 160;

struct System_s { };

struct Sync_s : System_s {
    virtual void fill_buffer(char*) = 0;
    virtual void consume_buffer(char*) = 0;
    virtual uint8_t get_buf_len() = 0;
    void enqueue();

    static void set_socket(SOCKET sock);

    static void begin(sockaddr_in* addr);
    static void end();
    static void dispatch();
    static void final_dispatch();

    virtual EClass get_eclass() = 0;

    static SOCKET s_sock;
    static sockaddr_in* s_address;

    static char s_data_buf[MAX_BUF_SIZE];
    static uint16_t s_data_len;

    static char s_head_buf[MAX_BUF_SIZE];
    static uint16_t s_head_len;

    static uint8_t s_num_ents;
};

struct Render_s : System_s {
    virtual void render(vec3 origin) const = 0;
};

struct Tick_s : System_s {
    virtual void tick(double dt) = 0;
};

#endif