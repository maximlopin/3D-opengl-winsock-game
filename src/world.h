#ifndef WORLD_H
#define WORLD_H

#include "stdlib.h"
#include "stdint.h"
#include "entity.h"
#include "uid_map.h"

static const int MAX_ENTS_PER_CLASS = 100;

struct World {
    World();

    /* Sync-server */
    uid_map<Hero_e> heroes = uid_map<Hero_e>(MAX_ENTS_PER_CLASS);
    uid_map<Monster_e> monsters = uid_map<Monster_e>(MAX_ENTS_PER_CLASS);
    uid_map<DroppedItem_e> dropped_items = uid_map<DroppedItem_e>(MAX_ENTS_PER_CLASS);

    /* Non-Sync-server */
    uid_map<Prop_e> props = uid_map<Prop_e>(MAX_ENTS_PER_CLASS);
    uid_map<NPC_e> npcs = uid_map<NPC_e>(MAX_ENTS_PER_CLASS);
    uid_map<Portal_e> portals = uid_map<Portal_e>(MAX_ENTS_PER_CLASS);

    // /* Sync-client */
    // uid_array<Hero_e> heroes = uid_array<Hero_e>(MAX_ENTS_PER_CLASS);
    // uid_array<Monster_e> monsters = uid_array<Monster_e>(MAX_ENTS_PER_CLASS);
    // uid_array<DroppedItem_e> dropped_items = uid_array<DroppedItem_e>(MAX_ENTS_PER_CLASS);

    // /* Non-Sync-client */
    // uid_array<Prop_e> props = uid_array<Prop_e>(MAX_ENTS_PER_CLASS);
    // uid_array<NPC_e> npcs = uid_array<NPC_e>(MAX_ENTS_PER_CLASS);
    // uid_array<Portal_e> portals = uid_array<Portal_e>(MAX_ENTS_PER_CLASS);
};

#endif