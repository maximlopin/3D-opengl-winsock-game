#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include "cglm/cglm.h"
#include "model.h"
#include "logging.h"

struct Component_c { };

struct Model_c : Component_c {
    Model* model;
    float scale = 1.0f;
    vec3 offset = { 0.0f };
};

struct Position_c : Component_c {
    float elevation = 0.0f;
    vec2 pos = { 0.0f };
    float distance_from(Position_c& pos_c);
};

struct Velocity_c : Component_c {
    vec2 vel = { 0.0f };
    float min = 4.0f;
    float max = 8.0f;
};

/*
    A renderable 3D object without physics,
    consisting of one or multiple transformed models
*/
struct Mesh_c : Component_c {
    virtual void render(vec3 origin, vec3 pos) = 0;
};

struct TreeMesh_c : Mesh_c {
    TreeMesh_c();
    ~TreeMesh_c();
    virtual void render(vec3 origin, vec3 pos) override;
    Model* m_tree_model;
};

#endif
