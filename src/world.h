#ifndef WORLD_H
#define WORLD_H

#include "stdlib.h"
#include "stdint.h"
#include "entity.h"

template<typename T>
struct uid_map {
    uid_map(int32_t init_size)
    {
        m_array = (T*) calloc(init_size, sizeof(T));
    }

    T& operator [](int32_t i)
    {
        return (T&) m_array[i];
    }

    ~uid_map()
    {
        free(m_array);
    }

    void add(T& item)
    {
        m_array[m_size++] = item;
        memcpy(m_array[m_size++], item, sizeof(T));
    }

    void del(int32_t i)
    {
        memcpy(m_array[i], m_array[--m_size], sizeof(T));
    }


    int32_t size()
    {
        return m_size;
    }
private:
    T* m_array;
    int32_t m_size = 0;
};

template<typename T>
struct uid_array {
    uid_array(int32_t init_size)
    {
        m_array = (T*) calloc(init_size, sizeof(T));
    }

    T& operator [](int32_t i)
    {
        return (T&) m_array[i];
    }

    ~uid_array()
    {
        free(m_array);
    }

private:
    T* m_array;
};

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

    /* Sync-client */
    uid_array<Hero_e> heroes = uid_array<Hero_e>(MAX_ENTS_PER_CLASS);
    uid_array<Monster_e> monsters = uid_array<Monster_e>(MAX_ENTS_PER_CLASS);
    uid_array<DroppedItem_e> dropped_items = uid_array<DroppedItem_e>(MAX_ENTS_PER_CLASS);

    /* Non-Sync-client */
    uid_array<Prop_e> props = uid_array<Prop_e>(MAX_ENTS_PER_CLASS);
    uid_array<NPC_e> npcs = uid_array<NPC_e>(MAX_ENTS_PER_CLASS);
    uid_array<Portal_e> portals = uid_array<Portal_e>(MAX_ENTS_PER_CLASS);
};

#endif