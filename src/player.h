#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include "winsock2.h"
#include "input.h"
#include "entity.h"
#include "world.h"

/* Represents a connection AND a hero entity.
   A hero must not exist without a connection. */
struct Player {
    Player(sockaddr_in addr, World& world);
    sockaddr_in addr;
    Input input;
    int32_t m_hero_id;
    static void create(sockaddr_in addr, World& world);
    static std::unordered_map<std::string, Player*> s_players;
};

#endif