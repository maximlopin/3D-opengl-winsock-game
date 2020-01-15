#include "world.h"

void World::add_player(Player* p)
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        if (this->players[i] == NULL)
        {
            this->players[i] = p;
            this->num_players++;
            break;
        }
    }
    
}

void World::del_player(sockaddr_in* addr)
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        if (this->players[i] == NULL) continue;

        if (cmp_addr(&(this->players[i]->addr), addr))
        {
            this->players[i] = NULL;
            this->num_players--;
        }
    }
}

Player::Player(sockaddr_in addr)
{
    this->addr = addr;
    this->load_data();
}

void Player::create(sockaddr_in addr)
{
    std::string ip_str = std::string(inet_ntoa(addr.sin_addr));
    std::string port_str = std::to_string(ntohs(addr.sin_port));
    std::string key =  ip_str + ":" + port_str;
    
    if (s_players.find(key) == s_players.end())
    {
        s_players[key] = new Player(addr);
        std::cout << key << "connected" << std::endl;
    }
    else
    {
        std::cout << key << "connected (ignoring repeating connection)" << std::endl;
    }
}

void Player::load_data()
{
    /* TODO */
}

void Player::tick(double dt)
{
    if (this->input.RM_PRESSED)
    {
        this->hero.vel.vel[0] = sinf(this->input.cursor_theta) * this->hero.vel.max * this->input.RM_PRESSED;
        this->hero.vel.vel[1] = -cosf(this->input.cursor_theta) * this->hero.vel.max * this->input.RM_PRESSED;
    }
    else if (this->input.LM_PRESSED)
    {
        this->hero.vel.vel[0] = sinf(this->input.cursor_theta) * this->hero.vel.min * this->input.LM_PRESSED;
        this->hero.vel.vel[1] = -cosf(this->input.cursor_theta) * this->hero.vel.min * this->input.LM_PRESSED;
    }

    this->hero.pos.pos[0] += this->hero.vel.vel[0] * dt;
    this->hero.pos.pos[1] += this->hero.vel.vel[1] * dt;
}


std::unordered_map<std::string, Player*> Player::s_players = {};