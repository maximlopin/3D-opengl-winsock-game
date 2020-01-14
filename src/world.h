#ifndef WORLD_H
#define WORLD_H

#include "cglm/cglm.h"
#include "entity.h"

#define MAX_SIZE 1024

/* Represents a connection AND a hero entity.
   A hero must not exist without a connection. */
struct Player {
    sockaddr_in addr;
    Hero_e* hero;
    void load_data();
};

struct World {

    /* Dynamic */
    Player* players[MAX_SIZE] = { NULL };
    uint16_t num_players = 0;

    Monster_e* monsters[MAX_SIZE] = { NULL };
    uint16_t num_monsters = 0;

    DroppedItem_e* dropped_items[MAX_SIZE] = { NULL };
    uint16_t num_dropped_items = 0;

    /* Static */
    Prop_e* props = NULL;
    uint16_t num_props = 0;

    NPC_e* npcs = NULL;
    uint16_t num_npcs = 0;

    Portal_e* portals = NULL;
    uint16_t num_portals = 0;

    void add_player(Player* p);
    void del_player(sockaddr_in* addr);

};

static int cmp_addr(sockaddr_in* a, sockaddr_in* b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) && (a->sin_port == b->sin_port);
}

#endif