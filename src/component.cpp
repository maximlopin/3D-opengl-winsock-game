#include "component.h"

float Position_c::distance_from(Position_c& pos_c)
{
    float x_diff = this->pos[0] - pos_c.pos[0];
    float y_diff = this->pos[1] - pos_c.pos[1];
    return (x_diff * x_diff) + (y_diff * y_diff);
}

TreeMesh_c::TreeMesh_c()
{
    /* Create required models */
    m_tree_model = new Model("models/tree.ply.bin");
}

TreeMesh_c::~TreeMesh_c()
{
    delete m_tree_model;
}

void TreeMesh_c::render(vec3 origin, vec3 pos)
{
    /* Transform and render models */
    m_tree_model->render(origin, pos);
}

TerrainMesh_c::TerrainMesh_c()
{
    /* Create required models */
    m_model = new Model("models/surface.ply.bin");
}

TerrainMesh_c::~TerrainMesh_c()
{
    delete m_model;
}

void TerrainMesh_c::render(vec3 origin, vec3 pos)
{
    /* Transform and render models */
    m_model->render(origin, pos);
}