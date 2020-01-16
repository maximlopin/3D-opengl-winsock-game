#include "world.h"

World::World()
{

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

template<typename T>
uid_array<T>::uid_array(int32_t init_size)
{
    m_array = (T*) calloc(init_size, sizeof(T));
}


template<typename T>
T& uid_array<T>::operator [](int32_t i)
{
    return m_array[i];
}

template<typename T>
uid_array<T>::~uid_array()
{
    free(m_array);
}

template<typename T>
void uid_array<T>::add(T& p_item)
{
    m_array[m_size++] = p_item;
    memcpy(m_array[m_size++], p_item, sizeof(T));
}

template<typename T>
void uid_array<T>::del(int32_t i)
{
    memcpy(m_array[i], m_array[--m_size], sizeof(T));
}

template<typename T>
int32_t uid_array<T>::size()
{
    return m_size;
}
