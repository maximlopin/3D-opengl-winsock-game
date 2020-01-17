#include "component.h"

float Position_c::distance_from(Position_c& pos_c)
{
    float x_diff = this->pos[0] - pos_c.pos[0];
    float y_diff = this->pos[1] - pos_c.pos[1];
    return (x_diff * x_diff) + (y_diff * y_diff);
}