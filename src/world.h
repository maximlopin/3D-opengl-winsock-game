#ifndef WORLD_H
#define WORLD_H

#include "stdlib.h"
#include "stdint.h"
#include "cglm/cglm.h"
#include "entity.h"
#include "input.h"
#include <unordered_map>
#include <string>
#include <iostream>

#define MAX_SIZE 1024


template<typename T>
struct uid_array {
    uid_array(int32_t init_size);
    T& operator [](int32_t i);
    ~uid_array();
    void add(T& p_item);
    void del(int32_t i);
    int32_t size();
private:
    T* m_array;
    int32_t m_size = 0;
};

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
    World();

    /* Sync */
    uid_array<Hero_e> heroes = uid_array<Hero_e>(100);
    uid_array<Monster_e> monsters = uid_array<Monster_e>(100);
    uid_array<DroppedItem_e> dropped_items = uid_array<DroppedItem_e>(100);

    /* Non-Sync */
    uid_array<Prop_e> props = uid_array<Prop_e>(100);
    uid_array<NPC_e> npcs = uid_array<NPC_e>(100);
    uid_array<Portal_e> portals = uid_array<Portal_e>(100);
};

static int cmp_addr(sockaddr_in* a, sockaddr_in* b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) && (a->sin_port == b->sin_port);
}

#endif