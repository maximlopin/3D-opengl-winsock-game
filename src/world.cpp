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

void Player::load_data()
{
    /* TODO */
}