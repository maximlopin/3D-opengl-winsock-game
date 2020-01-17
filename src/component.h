#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "cglm/cglm.h"
#include "model.h"

struct Component_c { };

struct Model_c : Component_c {
    Model* model;
    float scale;
    vec3 offset;
};

struct Position_c : Component_c {
    float elevation;
    vec2 pos;
    float distance_from(Position_c& pos_c);
};

struct Velocity_c : Component_c {
    vec2 vel;
    float min;
    float max;
};

#endif
