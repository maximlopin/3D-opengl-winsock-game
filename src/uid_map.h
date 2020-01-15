#ifndef UID_MAP_H
#define UID_MAP_H

#include "entity.h"

struct uid_map {

    Entity_e* operator [](uint32_t id)
    {
        if (id < 0 || id >= m_size)
        {
            return m_array[id];
        }
        return nullptr;
    }

    void add(Entity_e* item)
    {
        for (int i = 0; i < m_max; i++)
        {
            if (m_array[i] == nullptr)
            {
                m_array[i] = item;
                break;
            }
        }
        std::swap()
    }

private:

    static const uint32_t m_max = 2048;
    Entity_e* m_array[m_max] = { nullptr };
    uint32_t m_size = 0;
};

#endif