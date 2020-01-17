#include "entity.h"

void Hero_e::fill_buffer(char* buf)
{
    memcpy(buf, this->pos.pos, sizeof(this->pos.pos));
}

void Hero_e::consume_buffer(char* buf)
{
    memcpy(this->pos.pos, buf, sizeof(this->pos.pos));
}

uint8_t Hero_e::get_buf_len()
{
    return sizeof(this->pos.pos);
}

void Hero_e::tick(double dt)
{

}

void Hero_e::render(vec3 origin) const
{
    
}

void Monster_e::fill_buffer(char* buf)
{
    memcpy(buf, this->pos.pos, sizeof(this->pos.pos));
}

void Monster_e::consume_buffer(char* buf)
{
    memcpy(this->pos.pos, buf, sizeof(this->pos.pos));
}

uint8_t Monster_e::get_buf_len()
{
    return 0;
}

void Monster_e::tick(double dt)
{

}

void Monster_e::render(vec3 origin) const
{
    
}

void DroppedItem_e::fill_buffer(char* buf)
{
    memcpy(buf, this->pos.pos, sizeof(this->pos.pos));
}

void DroppedItem_e::consume_buffer(char* buf)
{
    memcpy(this->pos.pos, buf, sizeof(this->pos.pos));
}

uint8_t DroppedItem_e::get_buf_len()
{
    return sizeof(this->pos.pos);
}


void DroppedItem_e::tick(double dt)
{

}

void DroppedItem_e::render(vec3 origin) const
{
    
}

void NPC_e::render(vec3 origin) const
{
    
}

void Portal_e::render(vec3 origin) const
{
    
}
