#include "player.h"

Player::Player(sockaddr_in addr, World& world)
{
    Hero_e hero();
    m_hero_id = world.heroes.add(&hero);
    this->addr = addr;
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