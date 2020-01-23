#include "system.h"

void Sync_s::begin(SOCKET sock, sockaddr_in* addr_ptr)
{
    s_sock = sock;
    s_address_ptr = addr_ptr;
}

void Sync_s::end()
{
    if (s_num_ents > 0)
    {
        dispatch();
    }
}

void Sync_s::dispatch()
{
    int32_t offset = 0;

    memcpy(s_buf + offset, &s_num_ents, sizeof(s_num_ents));
    offset += sizeof(s_num_ents);

    memcpy(s_buf + offset, s_einfo_buf, s_einfo_len);
    offset += s_einfo_len;

    memcpy(s_buf + offset, s_edata_buf, s_edata_len);
    offset += s_edata_len;

    sendto(s_sock, reinterpret_cast<char*>(s_buf), offset, 0, (sockaddr*) s_address_ptr, sizeof(sockaddr));

    // INFO("Dispatched a packet of size " << offset << " (" << static_cast<int>(s_num_ents) << " entities)");

    s_einfo_len = 0;
    s_edata_len = 0;
    s_num_ents = 0;
}

void Sync_s::enqueue(int32_t eclass, int32_t id)
{
    int32_t len = get_buf_len();
    int32_t overhead = len + sizeof(eclass) + sizeof(id);

    if ((sizeof(s_num_ents) + s_einfo_len + s_edata_len + overhead) > MAX_PACKET_SIZE)
    {
        dispatch();
    }

    memcpy(s_einfo_buf + s_einfo_len, &eclass, sizeof(eclass));
    s_einfo_len += sizeof(eclass);

    memcpy(s_einfo_buf + s_einfo_len, &id, sizeof(id));
    s_einfo_len += sizeof(id);

    fill_buffer(s_edata_buf + s_edata_len);
    s_edata_len += len;

    s_num_ents++;

    // INFO("Entity (EClass = " << eclass << ", id = " << id << ") has been enqueued");
}

SOCKET Sync_s::s_sock = INVALID_SOCKET;
sockaddr_in* Sync_s::s_address_ptr = NULL;

int8_t Sync_s::s_buf[MAX_BUF_SIZE] = { 0 };

int32_t Sync_s::s_num_ents = 0;

int8_t Sync_s::s_einfo_buf[MAX_BUF_SIZE] = { 0 };
int32_t Sync_s::s_einfo_len = 0;

int8_t Sync_s::s_edata_buf[MAX_BUF_SIZE] = { 0 };
int32_t Sync_s::s_edata_len = 0;
