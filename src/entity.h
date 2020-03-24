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
    Movement_c m_mov;

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

#endif