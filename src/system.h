#ifndef SYSTEM_H
#define SYSTEM_H

#include "stdint.h"
#include "winsock2.h"
#include "cglm/cglm.h"
#include "eclass.h"
#include "logging.h"

static const int32_t MAX_BUF_SIZE = ((2 << 15) - 1);
static const int32_t MAX_PACKET_SIZE = 160;

struct System_s { };

struct Sync_s : System_s {
    virtual void fill_buffer(int8_t*) = 0;
    virtual void consume_buffer(int8_t*) = 0;
    virtual int32_t get_buf_len() = 0;
    void enqueue(int32_t eclass, int32_t id);

    static void begin(SOCKET sock, sockaddr_in* addr_ptr);
    static void end();
    static void dispatch();
    static void final_dispatch();

    static SOCKET s_sock;
    static sockaddr_in* s_address_ptr;

    static int8_t s_buf[MAX_BUF_SIZE];

    static int32_t s_num_ents;

    static int8_t s_einfo_buf[MAX_BUF_SIZE];
    static int32_t s_einfo_len;

    static int8_t s_edata_buf[MAX_BUF_SIZE];
    static int32_t s_edata_len;
};

struct Render_s : System_s {
    virtual void render(vec3 origin) const = 0;
};

struct Tick_s : System_s {
    virtual void tick(double dt) = 0;
};

#endif