#include "entity.h"

void DynamicEntity_e::set_socket(SOCKET sock)
{
    s_sock = sock;
}

void DynamicEntity_e::begin(sockaddr_in* addr)
{
    s_address = addr;
}

/* Must be called after enqueing data for a client */
void DynamicEntity_e::end()
{
    if (s_num_ents > 0)
    {
        dispatch();
    }
}

void DynamicEntity_e::dispatch()
{
    /* Share the same buffer for all entities */
    static char buf[MAX_BUF_SIZE];

    memcpy(buf, &s_num_ents, 0);
    memcpy(buf + sizeof(s_num_ents), s_head_buf, s_head_len);
    memcpy(buf + sizeof(s_num_ents) + s_head_len, s_data_buf, s_data_len);
    sendto(s_sock, buf, sizeof(s_num_ents) + s_data_len + s_head_len, 0, (sockaddr*) s_address, sizeof(sockaddr));
}

void DynamicEntity_e::enqueue()
{
    /* Share the same buffer for all entities */
    static char buf[MAX_BUF_SIZE];

    EClass eclass = this->get_eclass();

    int len = this->fill_buffer(buf);

    uint16_t packet_size = (sizeof(eclass) + s_head_len) + (s_data_len + len) + sizeof(s_num_ents);

    if (packet_size > MAX_PACKET_SIZE)
    {
        dispatch();
        s_head_len = 0;
        s_data_len = 0;
        s_num_ents = 0;
    }

    memcpy(s_head_buf + s_head_len, &eclass, sizeof(eclass));
    s_head_len += sizeof(eclass);

    memcpy(s_data_buf + s_data_len, buf, len);
    s_data_len += len;

    s_num_ents++;
}

int Monster_e::fill_buffer(char* buf)
{
    int len = sizeof(this->pos.pos);
    memcpy(buf, this->pos.pos, len);
    return len;
}

int Hero_e::fill_buffer(char* buf)
{
    int len = sizeof(this->pos.pos);
    memcpy(buf, this->pos.pos, len);
    return len;
}

int DroppedItem_e::fill_buffer(char* buf)
{
    int len = sizeof(this->pos.pos);
    memcpy(buf, this->pos.pos, len);
    return len;
}

void Monster_e::consume_buffer(char* buf)
{
    memcpy(this->pos.pos, buf, sizeof(this->pos.pos));
}

void Hero_e::consume_buffer(char* buf)
{
    memcpy(this->pos.pos, buf, sizeof(this->pos.pos));
}

void DroppedItem_e::consume_buffer(char* buf)
{
    memcpy(this->pos.pos, buf, sizeof(this->pos.pos));
}