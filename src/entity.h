#ifndef ENTITY_H
#define ENTITY_H

#include "cglm/cglm.h"
#include "component.h"
#include "system.h"
#include "input.h"

struct Entity_e {
    Entity_e(int32_t id);
    int32_t get_id();
private:
    const int32_t m_id;
};

struct Hero_e : Entity_e, Sync_s, Tick_s, Render_s {
    Hero_e();
    Hero_e(int32_t id);
    Input m_input;
    Model_c m_model;
    Position_c m_pos;
    Velocity_c m_vel;
    virtual void fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual uint8_t get_buf_len() override;
    virtual void render(vec3 origin) const override;
    virtual void tick(double dt) override;
};

struct Monster_e : Entity_e, Sync_s, Tick_s, Render_s {
    Monster_e(int32_t id);
    Model_c m_model;
    Position_c m_pos;
    Velocity_c m_vel;
    virtual void fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual uint8_t get_buf_len() override;
    virtual void render(vec3 origin) const override;
    virtual void tick(double dt) override;
};

struct DroppedItem_e : Entity_e, Sync_s, Tick_s, Render_s {
    DroppedItem_e(int32_t id);
    Position_c m_pos;
    Model_c m_model;
    virtual void fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual uint8_t get_buf_len() override;
    virtual void render(vec3 origin) const override;
    virtual void tick(double dt) override;
};

struct Prop_e : Entity_e, Render_s {
    Prop_e(int32_t id);
    Model_c m_model;
    Position_c m_pos;
    virtual void render(vec3 origin) const override;
};

struct NPC_e : Entity_e, Render_s {
    NPC_e(int32_t id);
    Model_c m_model;
    Position_c m_pos;
    virtual void render(vec3 origin) const override;
};

struct Portal_e : Entity_e, Render_s {
    Portal_e(int32_t id);
    Position_c m_pos;
    virtual void render(vec3 origin) const override;
};

#endif