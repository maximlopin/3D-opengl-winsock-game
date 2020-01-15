#ifndef WORLD_H
#define WORLD_H

#include "stdint.h"
#include "cglm/cglm.h"
#include "entity.h"
#include "input.h"
#include <unordered_map>
#include <string>
#include <iostream>

#define MAX_SIZE 1024

/* Represents a connection AND a hero entity.
   A hero must not exist without a connection. */
struct Player {
    Player(sockaddr_in addr);
    sockaddr_in addr;
    Hero_e hero;
    Input input;
    void tick(double dt);
    void load_data();
    static void create(sockaddr_in addr);
    static std::unordered_map<std::string, Player*> s_players;
};

struct World {

    /* Sync */
    std::unordered_map<uint32_t, Hero_e*> heroes;
    std::unordered_map<uint32_t, Monster_e*> monsters;
    std::unordered_map<uint32_t, DroppedItem_e*> dropped_items;

    /* Non-Sync */
    Prop_e* props = NULL;
    NPC_e* npcs = NULL;
    Portal_e* portals = NULL;
};

static int cmp_addr(sockaddr_in* a, sockaddr_in* b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) && (a->sin_port == b->sin_port);
}

#endif