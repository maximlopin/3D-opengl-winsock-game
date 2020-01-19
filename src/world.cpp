#include "world.h"

static int cmp_addr(sockaddr_in* a, sockaddr_in* b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) && (a->sin_port == b->sin_port);
}

std::string addr_to_string(sockaddr_in& addr)
{
    std::string ip_str = std::string(inet_ntoa(addr.sin_addr));
    std::string port_str = std::to_string(ntohs(addr.sin_port));
    std::string str =  ip_str + ":" + port_str;
    return str;
}

World::World()
{

}

Player::Player(sockaddr_in addr, World& world)
{
    int32_t hero_id = world.m_heroes.new_id();

    Hero_e hero(hero_id);

    /* Init hero here */
    /*       ...      */
    /* -------------- */

    world.m_heroes.set(hero_id, &hero);

    m_addr = addr;
    m_hero_id = hero_id;
    m_world_ptr = &world;
}

Player::~Player()
{
    m_world_ptr->m_heroes.del(m_hero_id);
}

Player* Player::create(sockaddr_in addr, World& world)
{
    std::string key = addr_to_string(addr);
    
    if (s_players.find(key) == s_players.end())
    {
        Player* player_ptr = new Player(addr, world);
        s_players[key] = player_ptr;
        INFO(key << " connected");
        return player_ptr;
    }

    INFO(key << " already is connected");
    return nullptr;
}

std::unordered_map<std::string, Player*> Player::s_players = { };