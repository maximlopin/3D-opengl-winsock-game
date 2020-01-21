#ifndef ENTITY_H
#define ENTITY_H

#include "stdlib.h"
#include "winsock2.h"
#include "cglm/cglm.h"
#include "component.h"
#include "system.h"
#include "input.h"
#include "logging.h"

struct Entity_e {
    Entity_e(int32_t id);
    int32_t get_id();
private:
    const int32_t m_id;
};

struct Hero_e : Entity_e, Sync_s, Tick_s {
    Hero_e(int32_t id);
    Input m_input;
    Position_c m_pos;
    Velocity_c m_vel;

#ifdef CLIENT
    TreeMesh_c m_mesh;
#endif

    virtual void fill_buffer(int8_t* buf) override;
    virtual void consume_buffer(int8_t* buf) override;
    virtual int32_t get_buf_len() override;
    virtual void tick(double dt) override;
    int32_t fill_input_buffer(int8_t* buf);
    void consume_input_buffer(int8_t* buf);
};

struct Monster_e : Entity_e, Sync_s, Tick_s {
    Monster_e(int32_t id);
    Model_c m_model;
    Position_c m_pos;
    Velocity_c m_vel;
    virtual void fill_buffer(int8_t*) override;
    virtual void consume_buffer(int8_t*) override;
    virtual int32_t get_buf_len() override;
    virtual void tick(double dt) override;
};

struct DroppedItem_e : Entity_e, Sync_s, Tick_s {
    DroppedItem_e(int32_t id);
    Position_c m_pos;
    Model_c m_model;
    virtual void fill_buffer(int8_t*) override;
    virtual void consume_buffer(int8_t*) override;
    virtual int32_t get_buf_len() override;
    virtual void tick(double dt) override;
};

struct Prop_e : Entity_e {
    Prop_e(int32_t id);
    Model_c m_model;
    Position_c m_pos;
};

struct NPC_e : Entity_e {
    NPC_e(int32_t id);
    Model_c m_model;
    Position_c m_pos;
};

struct Portal_e : Entity_e {
    Portal_e(int32_t id);
    Position_c m_pos;
};

#endif