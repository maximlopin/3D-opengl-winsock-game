#ifndef WORLD_H
#define WORLD_H

#include <iostream>
#include <unordered_map>
#include <string>
#include "winsock2.h"
#include "input.h"
#include "entity.h"
#include "stdint.h"
#include "uid_map.h"
#include "logging.h"

static const int MAX_ENTS_PER_CLASS = 100;

static int cmp_addr(sockaddr_in* a, sockaddr_in* b);
std::string addr_to_string(sockaddr_in& addr);

struct World {
    World();

#ifdef CLIENT
    TerrainMesh_c m_mesh;
#endif
    EMAP<Hero_e> m_heroes = EMAP<Hero_e>(MAX_ENTS_PER_CLASS);
};

#ifdef SERVER
struct Player {
    Player(sockaddr_in addr, World& world);
    ~Player();
    sockaddr_in m_addr;
    int32_t m_hero_id;
    World* m_world_ptr;
    static Player* create(sockaddr_in addr, World& world);
    static std::unordered_map<std::string, Player*> s_players;

    static void distribute_packets(SOCKET sock)
    {
        for (auto keyval : Player::s_players)
        {
            Player* player_ptr = keyval.second;
            World* world_ptr = player_ptr->m_world_ptr;

            // INFO("Sending packets to " << keyval.first);

            Sync_s::begin(sock, &(player_ptr->m_addr));

            for (int i = 0; i < world_ptr->m_heroes.size(); i++)
            {
                Hero_e* hero_ptr = world_ptr->m_heroes.by_index(i);
                EClass eclass = EClass::ECLASS_HERO;
                int32_t id = hero_ptr->get_id();
                world_ptr->m_heroes.by_index(i)->enqueue(eclass, id);
            }

            Sync_s::end();
        }       
    }
};
#endif

#endif