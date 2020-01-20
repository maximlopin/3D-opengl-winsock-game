#ifndef UID_MAP_H
#define UID_MAP_H

#include "stdlib.h"
#include "stdint.h"

struct filled_array {
    filled_array(int32_t init_size, int32_t fill) : m_size(init_size)
    {
        m_array = (int32_t*) calloc(init_size, sizeof(int32_t));
        for (int i = 0; i < init_size; i++)
        {
            m_array[i] = fill;
        }
    }

    int32_t& operator [](int32_t i)
    {
        return m_array[i];
    }
    
    ~filled_array()
    {
        free(m_array);
    }

    const int32_t m_size;
    int32_t* m_array;
};

struct index_set {
    index_set(int32_t init_size) : m_size(init_size)
    {
        m_array = (int32_t*) calloc(init_size, sizeof(int32_t));
        for (int i = 0; i < init_size; i++)
        {
            m_array[i] = init_size - i - 1;
        }
    }

    ~index_set()
    {
        free(m_array);
    }

    void put(int32_t i)
    {
        m_array[m_size++] = i;
    }

    int32_t pop()
    {
        return m_array[--m_size];
    }

    int32_t m_size;
    int32_t* m_array;
};

template<typename T>
struct uid_map {
    uid_map(int32_t init_size) : m_init_size(init_size)
    {
        m_array = (T*) calloc(init_size, sizeof(T));
    }

    T* by_id(int32_t id)
    {
        if (m_idtoi[id] != -1)
        {
            return &(m_array[m_idtoi[id]]);
        }
        return nullptr;
    }

    T* by_index(int32_t i)
    {
        return &(m_array[i]);
    }

    ~uid_map()
    {
        free(m_array);
    }

    /* Returns id */
    int32_t push(T* item_ptr)
    {
        int32_t id = m_free_ids.pop();
        m_idtoi[id] = m_size;
        m_itoid[m_size] = id;
        memcpy(&(m_array[m_size++]), item_ptr, sizeof(T));
        return id;
    }

    void force_add(int32_t id, T* item_ptr)
    {
        m_idtoi[id] = m_size;
        m_itoid[m_size++] = id;
        set(id, item_ptr);
    }

    void force_del(int32_t id)
    {
        memcpy(&(m_array[m_idtoi[id]]), &(m_array[--m_size]), sizeof(T));
        m_idtoi[m_itoid[m_size]] = m_idtoi[id];
    }

    int32_t new_id()
    {
        int32_t id = m_free_ids.pop();
        m_idtoi[id] = m_size;
        m_itoid[m_size++] = id;
        return id;
    }

    void set(int32_t id, T* item_ptr)
    {
        memcpy(&(m_array[m_idtoi[id]]), item_ptr, sizeof(T));
    }

    /* Deletes by static index */
    void del(int32_t id)
    {
        m_free_ids.put(id);
        memcpy(&(m_array[m_idtoi[id]]), &(m_array[--m_size]), sizeof(T));
        m_idtoi[m_itoid[m_size]] = m_idtoi[id];
    }

    int32_t itoid(int32_t i)
    {
        return m_itoid[i];
    }

    int32_t size()
    {
        return m_size;
    }
private:
    const int32_t m_init_size;

    T* m_array;
    int32_t m_size = 0;

    index_set m_free_ids = index_set(m_init_size);
    filled_array m_idtoi = filled_array(m_init_size, -1);
    filled_array m_itoid = filled_array(m_init_size, -1);
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

#endif