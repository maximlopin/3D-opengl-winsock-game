#include "entity.h"
#include "logging.h"

Entity_e::Entity_e(int32_t id) : m_id(id)
{

}

int32_t Entity_e::get_id()
{
    return m_id;
}

Hero_e::Hero_e(int32_t id) : Entity_e(id)
{

}

void Hero_e::fill_buffer(char* buf)
{
    memcpy(buf, m_pos.pos, sizeof(m_pos.pos));
}

void Hero_e::consume_buffer(char* buf)
{
    memcpy(m_pos.pos, buf, sizeof(m_pos.pos));
}

uint8_t Hero_e::get_buf_len()
{
    return sizeof(m_pos.pos);
}

void Hero_e::tick(double dt)
{
    if (m_input.RM_PRESSED)
    {
        m_vel.vel[0] = sinf(m_input.cursor_theta) * m_vel.max * m_input.RM_PRESSED;
        m_vel.vel[1] = -cosf(m_input.cursor_theta) * m_vel.max * m_input.RM_PRESSED;
    }
    else if (m_input.LM_PRESSED)
    {
        m_vel.vel[0] = sinf(m_input.cursor_theta) * m_vel.min * m_input.LM_PRESSED;
        m_vel.vel[1] = -cosf(m_input.cursor_theta) * m_vel.min * m_input.LM_PRESSED;
    }

    m_pos.pos[0] += m_vel.vel[0] * dt;
    m_pos.pos[1] += m_vel.vel[1] * dt;
}

void Hero_e::render(vec3 origin) const
{
    
}

Monster_e::Monster_e(int32_t id) : Entity_e(id)
{

}

void Monster_e::fill_buffer(char* buf)
{
    memcpy(buf, m_pos.pos, sizeof(m_pos.pos));
}

void Monster_e::consume_buffer(char* buf)
{
    memcpy(m_pos.pos, buf, sizeof(m_pos.pos));
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

DroppedItem_e::DroppedItem_e(int32_t id) : Entity_e(id)
{

}

void DroppedItem_e::fill_buffer(char* buf)
{
    memcpy(buf, m_pos.pos, sizeof(m_pos.pos));
}

void DroppedItem_e::consume_buffer(char* buf)
{
    memcpy(m_pos.pos, buf, sizeof(m_pos.pos));
}

uint8_t DroppedItem_e::get_buf_len()
{
    return sizeof(m_pos.pos);
}


void DroppedItem_e::tick(double dt)
{

}

void DroppedItem_e::render(vec3 origin) const
{
    
}

NPC_e::NPC_e(int32_t id) : Entity_e(id)
{

}

void NPC_e::render(vec3 origin) const
{
    
}

Portal_e::Portal_e(int32_t id) : Entity_e(id)
{

}

void Portal_e::render(vec3 origin) const
{
    
}
