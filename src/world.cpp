#include "world.h"

static int cmp_addr(sockaddr_in* a, sockaddr_in* b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) && (a->sin_port == b->sin_port);
}

World::World()
{

}

Player::Player(sockaddr_in addr, World& world)
{
    int32_t id = world.m_heroes.new_id();

    /* Init hero here */
    Hero_e hero(id);
    /* -------------- */

    world.m_heroes.set(id, &hero);

    m_addr = addr;
    m_hero_id = id;
    m_world_ptr = &world;
}

Player::~Player()
{
    m_world_ptr->m_heroes.del(m_hero_id);
}

void Player::create(sockaddr_in addr, World& world)
{
    std::string ip_str = std::string(inet_ntoa(addr.sin_addr));
    std::string port_str = std::to_string(ntohs(addr.sin_port));
    std::string key =  ip_str + ":" + port_str;
    
    if (s_players.find(key) == s_players.end())
    {
        s_players[key] = new Player(addr, world);
        std::cout << key << " connected" << std::endl;
    }
    else
    {
        std::cout << key << " connected (ignoring repeating connection)" << std::endl;
    }
}

std::unordered_map<std::string, Player*> Player::s_players = { };