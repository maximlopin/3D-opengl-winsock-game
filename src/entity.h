#ifndef ENTITY_H
#define ENTITY_H

#include "stdint.h"
#include "winsock2.h"
#include "cglm/cglm.h"
#include "model.h"

static const uint16_t MAX_BUF_SIZE = ((2 << 15) - 1);
static const uint16_t MAX_PACKET_SIZE = 160;

/* Must always be sequential */
enum EClass {
    ECLASS_HERO,
    ECLASS_MONSTER,
    ECLASS_PROP,
    ECLASS_NPC,
    ECLASS_PORTAL,
    ECLASS_DROPPED_ITEM,
    ECLASS_LENGTH
};

typedef uint8_t EClass;

/* Suffix _c means component */

struct Component_c { };

struct Model_c : Component_c {
    Model* model;
    float scale;
    vec3 offset;
};

struct Position_c : Component_c {
    float elevation;
    vec2 pos;
};

struct Velocity_c : Component_c {
    vec2 vel;
    float velmin;
    float velmax;
};

/* Suffix _e means entity */

struct Entity_e { };

struct StaticEntity_e : Entity_e { };

/*
    Dynamic entity packet structure

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

struct DynamicEntity_e : Entity_e {
    virtual int fill_buffer(char*) = 0;
    virtual void consume_buffer(char*) = 0;
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

struct Hero_e : DynamicEntity_e {
    const EClass eclass = ECLASS_HERO;
    Model_c model;
    Position_c pos;
    Velocity_c vel;
    virtual int fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual EClass get_eclass() override;
};

struct Monster_e : DynamicEntity_e {
    const EClass eclass = ECLASS_MONSTER;
    Model_c model;
    Position_c pos;
    Velocity_c vel;
    virtual int fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual EClass get_eclass() override;
};

struct DroppedItem_e : DynamicEntity_e {
    const EClass eclass = ECLASS_DROPPED_ITEM;
    Position_c pos;
    Model_c model;
    virtual int fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual EClass get_eclass() override;
};

struct Prop_e : StaticEntity_e {
    const EClass eclass = ECLASS_PROP;
    Model_c model;
    Position_c pos;
};

struct NPC_e : StaticEntity_e {
    const EClass eclass = ECLASS_NPC;
    Model_c model;
    Position_c pos;
};

struct Portal_e : StaticEntity_e {
    const EClass eclass = ECLASS_PORTAL;
    Position_c pos;
};

#endif