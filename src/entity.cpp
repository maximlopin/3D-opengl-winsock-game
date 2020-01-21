#include "entity.h"

Entity_e::Entity_e(int32_t id) : m_id(id)
{
    INFO("Created entity with ID " << id);
}

int32_t Entity_e::get_id()
{
    return m_id;
}

Hero_e::Hero_e(int32_t id) : Entity_e(id)
{

}

void Hero_e::fill_buffer(int8_t* buf)
{
    memcpy(buf, m_pos.pos, sizeof(m_pos.pos));
}

void Hero_e::consume_buffer(int8_t* buf)
{
    memcpy(m_pos.pos, buf, sizeof(m_pos.pos));
}

int32_t Hero_e::get_buf_len()
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

    INFO("vel[0]: " << m_vel.vel[0]);
    INFO("vel[1]: " << m_vel.vel[1]);

    m_pos.pos[0] += m_vel.vel[0] * dt;
    m_pos.pos[1] += m_vel.vel[1] * dt;
}

int32_t Hero_e::fill_input_buffer(int8_t* buf)
{
    memcpy(buf, &m_input, sizeof(m_input));
    return sizeof(m_input);
}

void Hero_e::consume_input_buffer(int8_t* buf)
{
    memcpy(&m_input, buf, sizeof(m_input));
}

Monster_e::Monster_e(int32_t id) : Entity_e(id)
{

}

void Monster_e::fill_buffer(int8_t* buf)
{
    memcpy(buf, m_pos.pos, sizeof(m_pos.pos));
}

void Monster_e::consume_buffer(int8_t* buf)
{
    memcpy(m_pos.pos, buf, sizeof(m_pos.pos));
}

int32_t Monster_e::get_buf_len()
{
    return 0;
}

void Monster_e::tick(double dt)
{

}

DroppedItem_e::DroppedItem_e(int32_t id) : Entity_e(id)
{

}

void DroppedItem_e::fill_buffer(int8_t* buf)
{
    memcpy(buf, m_pos.pos, sizeof(m_pos.pos));
}

void DroppedItem_e::consume_buffer(int8_t* buf)
{
    memcpy(m_pos.pos, buf, sizeof(m_pos.pos));
}

int32_t DroppedItem_e::get_buf_len()
{
    return sizeof(m_pos.pos);
}


void DroppedItem_e::tick(double dt)
{

}

NPC_e::NPC_e(int32_t id) : Entity_e(id)
{

}

Portal_e::Portal_e(int32_t id) : Entity_e(id)
{

}
