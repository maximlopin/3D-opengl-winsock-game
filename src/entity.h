#ifndef ENTITY_H
#define ENTITY_H

#include "cglm/cglm.h"
#include "component.h"
#include "system.h"

struct Entity_e { };

struct Hero_e : Entity_e, Sync_s, Tick_s, Render_s {
    Model_c model;
    Position_c pos;
    Velocity_c vel;
    virtual void fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual uint8_t get_buf_len() override;
    virtual void render(vec3 origin) const override;
    virtual void tick(double dt) override;
};

struct Monster_e : Entity_e, Sync_s, Tick_s, Render_s {
    Model_c model;
    Position_c pos;
    Velocity_c vel;
    virtual void fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual uint8_t get_buf_len() override;
    virtual void render(vec3 origin) const override;
    virtual void tick(double dt) override;
};

struct DroppedItem_e : Entity_e, Sync_s, Tick_s, Render_s {
    Position_c pos;
    Model_c model;
    virtual void fill_buffer(char*) override;
    virtual void consume_buffer(char*) override;
    virtual uint8_t get_buf_len() override;
    virtual void render(vec3 origin) const override;
    virtual void tick(double dt) override;
};

struct Prop_e : Entity_e, Render_s {
    Model_c model;
    Position_c pos;
    virtual void render(vec3 origin) const override;
};

struct NPC_e : Entity_e, Render_s {
    Model_c model;
    Position_c pos;
    virtual void render(vec3 origin) const override;
};

struct Portal_e : Entity_e, Render_s {
    Position_c pos;
    virtual void render(vec3 origin) const override;
};

#endif