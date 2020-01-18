#ifndef ECLASS_H
#define ECLASS_H

#include "stdint.h"

/* Must always be sequential */
enum class EClass : uint8_t {
    ECLASS_HERO,
    ECLASS_LOCAL_HERO,
    ECLASS_MONSTER,
    ECLASS_PROP,
    ECLASS_NPC,
    ECLASS_PORTAL,
    ECLASS_DROPPED_ITEM,
    ECLASS_LENGTH
};

#endif