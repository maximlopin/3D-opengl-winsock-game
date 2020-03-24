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
    memcpy(buf, &m_mov, sizeof(m_mov));
}

void Hero_e::consume_buffer(int8_t* buf)
{
    memcpy(&m_mov, buf, sizeof(m_mov));
}

int32_t Hero_e::get_buf_len()
{
    return sizeof(m_mov);
}

void Hero_e::tick(double dt)
{
    if (m_input.RM_PRESSED)
    {
        m_mov.vel[0] = sinf(m_input.cursor_theta) * m_mov.max;
        m_mov.vel[1] = -cosf(m_input.cursor_theta) * m_mov.max;
    }
    else if (m_input.LM_PRESSED)
    {
        m_mov.vel[0] = sinf(m_input.cursor_theta) * m_mov.min;
        m_mov.vel[1] = -cosf(m_input.cursor_theta) * m_mov.min;
    }
    else
    {
        m_mov.vel[0] = 0.0f;
        m_mov.vel[1] = 0.0f;
    }

    m_mov.pos[0] += m_mov.vel[0] * dt;
    m_mov.pos[1] += m_mov.vel[1] * dt;
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
