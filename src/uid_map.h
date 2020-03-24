#ifndef UID_MAP_H
#define UID_MAP_H

#include "stdlib.h"
#include "stdint.h"

/*
    Contiguous storage where each item has a unique ID.
    The ID doesn't change for an entity unless you call
    del(int32_t id), but the position in the contiguous memory
    array can change for an item.
*/
template<typename T>
struct server_emap {
    server_emap(int32_t size)
    {
        m_num_ids = size;

        m_idtoi = (int32_t*) calloc(size, sizeof(int32_t));
        m_itoid = (int32_t*) calloc(size, sizeof(int32_t));
        m_free_ids = (int32_t*) calloc(size, sizeof(int32_t));

        for (int i = 0; i < size; i++)
        {
            m_idtoi[i] = -1;
            m_itoid[i] = -1;
            m_free_ids[i] = (size - i - 1);
        }

        m_items = (T*) calloc(size, sizeof(T));
    }

    ~server_emap()
    {
        free(m_idtoi);
        free(m_itoid);
        free(m_free_ids);
        free(m_items);
    }

    T* by_id(int32_t id)
    {
        int32_t index = m_idtoi[id];
        if (index == -1)
        {
            return nullptr;
        }
        return by_index(index);
    }

    T* by_index(int32_t index)
    {
        return &m_items[index];
    }

    int32_t get_free_id()
    {
        return m_free_ids[--m_num_ids];
    }

    void add_free_id(int32_t id)
    {
        m_free_ids[m_num_ids++] = id;
    }

    void set(int32_t id, T* item_ptr)
    {
        int32_t index = m_size++;
        assign(index, id);
        memcpy(&m_items[index], item_ptr, sizeof(T)); // use of deleted operator: m_items[index] = *item_ptr;
    }

    void add(T* item_ptr)
    {
        int32_t index = m_size++;
        int32_t id = get_free_id();
        assign(index, id);
        m_items[index] = *item_ptr;
    }

    void del(int32_t id)
    {
        int32_t index = m_idtoi[id];
        int32_t last_index = (m_size - 1);
        swap(index, last_index);
        pop();
    }

    void swap(int32_t index1, int32_t index2)
    {
        int32_t id1 = m_itoid[index1];
        int32_t id2 = m_itoid[index2];
        
        /* Actual swap */
        T item = m_items[index1];
        memcpy(&m_items[index1], &m_items[index2], sizeof(T)); // use of deleted operator: m_items[index1] = m_items[index2];
        memcpy(&m_items[index2], &item, sizeof(T)); // use of deleted operator: m_items[index2] = item;

        /* Fix ids and indices */
        assign(index1, id2);
        assign(index2, id1);
    }

    void pop()
    {
        int32_t last_index = (m_size - 1);
        int32_t last_id = m_itoid[last_index];

        m_itoid[last_index] = -1;
        m_idtoi[last_id] = -1;

        add_free_id(last_id);

        m_size--;
    }

    void assign(int32_t index, int32_t id)
    {
        m_idtoi[id] = index;
        m_itoid[index] = id;
    }

    int32_t size()
    {
        return m_size;
    }

    int32_t m_size = 0;

    int32_t* m_free_ids;
    int32_t m_num_ids;

    int32_t* m_idtoi;
    int32_t* m_itoid;

    T* m_items;
};

/*
    client_emap is only different from server_emap by not keeping
    track of free IDs. IDs are to be received from server when it
    sends entity packets.
*/
template<typename T>
struct client_emap {
    client_emap(int32_t size)
    {
        m_idtoi = (int32_t*) calloc(size, sizeof(int32_t));
        m_itoid = (int32_t*) calloc(size, sizeof(int32_t));

        for (int i = 0; i < size; i++)
        {
            m_idtoi[i] = -1;
            m_itoid[i] = -1;
        }

        m_items = (T*) calloc(size, sizeof(T));
    }

    ~client_emap()
    {
        free(m_idtoi);
        free(m_itoid);
        free(m_items);
    }

    T* by_id(int32_t id)
    {
        int32_t index = m_idtoi[id];
        if (index == -1)
        {
            return nullptr;
        }
        return by_index(index);
    }

    T* by_index(int32_t index)
    {
        return &m_items[index];
    }

    void set(int32_t id, T* item_ptr)
    {
        int32_t index = m_size++;
        assign(index, id);
        memcpy(&m_items[index], item_ptr, sizeof(T)); // use of deleted operator: m_items[index] = *item_ptr;
    }

    /* 
       Returns pointer to new empty location.
       WARNING: this pointer must be used before
       adding or removing items, because items
       can change their locations in memory after
       those operations.
    */
    T* allocate(int32_t id)
    {
        int32_t index = m_size++;
        assign(index, id);
        return &m_items[index];
    }

    void del(int32_t id)
    {
        int32_t index = m_idtoi[id];
        int32_t last_index = (m_size - 1);
        swap(index, last_index);
        pop();
    }

    void swap(int32_t index1, int32_t index2)
    {
        int32_t id1 = m_itoid[index1];
        int32_t id2 = m_itoid[index2];
        
        /* Actual swap */
        T item = m_items[index1];
        memcpy(&m_items[index1], &m_items[index2], sizeof(T)); // use of deleted operator: m_items[index1] = m_items[index2];
        memcpy(&m_items[index2], &item, sizeof(T)); // use of deleted operator: m_items[index2] = item;

        /* Fix ids and indices */
        assign(index1, id2);
        assign(index2, id1);
    }

    void pop()
    {
        int32_t last_index = (m_size - 1);
        int32_t last_id = m_itoid[last_index];

        m_itoid[last_index] = -1;
        m_idtoi[last_id] = -1;

        m_size--;
    }

    void assign(int32_t index, int32_t id)
    {
        m_idtoi[id] = index;
        m_itoid[index] = id;
    }

    int32_t size()
    {
        return m_size;
    }

    int32_t m_size = 0;

    int32_t* m_idtoi;
    int32_t* m_itoid;

    T* m_items;
};

#ifdef SERVER
    #define EMAP server_emap
#endif

#ifdef CLIENT
    #define EMAP client_emap
#endif

#endif