#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include "winsock2.h"
#include "input.h"
#include "entity.h"

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

#endif