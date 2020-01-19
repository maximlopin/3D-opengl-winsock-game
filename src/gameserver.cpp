#include "winsock2.h"
#include "addr.h"
#include "world.h"
#include <iostream>
#include <thread>
#include <mutex>
#include "auth.h"
#include "logging.h"

#define MAX_BUFFER ((2 << 15) - 1)

#define SOCKET_ERR_MSG(msg); fprintf(stderr, "%s (%s) (error %i)\n", msg, __func__, WSAGetLastError());

#define SLEEP(t0, freq) \
        double t1 = glfwGetTime(); \
        double dt = (t1 - t0); \
        double sleep_s = (1 / freq) - dt; \
        long long sleep_ms = static_cast<long long>(sleep_s * 1000); \
        if (sleep_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)); \

/*
    Entity packet structure

    -------------------------------
    uint8_t |  N (number of entities)
    -------------------------------
    uint8_t |
       .    |
       .    |  Each of N uint8_t are
       .    |  members of EClass struct.
       N    |
    -------------------------------
      BYTES | Some data, each entity
        .   | from above header is
        .   | responsible for consuming
        .   | it.
        M   |
    -------------------------------
*/

static const double PACKETS_FREQ = 1.0;
static const double TICK_FREQ = 1.0;

static World world;

/* Iterates through players and sends them entity data */
void _main_data()
{
    static char buf[MAX_BUFFER];

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    while (true)
    {
        double t0 = glfwGetTime();
        Player::distribute_packets(sock);
        SLEEP(t0, PACKETS_FREQ);
    }
}

/* Adds new players to world for new connections */
void _main_auth()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in name = SERVER_AUTH_ADDR;

    if (bind(sock, reinterpret_cast<sockaddr*>(&name), sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return;
    }

    while (true)
    {
        char buf[MAX_BUF_SIZE];

        sockaddr_in from;
        int fromlen = sizeof(sockaddr);

        if (recvfrom(sock, buf, MAX_BUF_SIZE, 0, reinterpret_cast<sockaddr*>(&from), &fromlen) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to receive auth packet");
            closesocket(sock);
            return;
        }

        /*
            Auth packet structure

            ----------------------------------------------
             int32_t | ID of newly created Hero_e instance
            ----------------------------------------------
        */

        Player* player_ptr = Player::create(from, world);

        int32_t id = player_ptr == nullptr ? -1 : player_ptr->m_hero_id;

        if (sendto(sock, reinterpret_cast<char*>(&id), sizeof(int32_t), 0, reinterpret_cast<sockaddr*>(&from), sizeof(sockaddr)) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to send ID");
            closesocket(sock);
            return;
        }
    }
    closesocket(sock);
}

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Failed to initialize WSA\n");
        return 1;
    }

    std::thread auth_thread(_main_auth);
    std::thread data_thread(_main_data);

    auth_thread.join();
    data_thread.join();

    for (auto keyval : Player::s_players)
    {
        Player* p_player = keyval.second;
        delete p_player;
    }

    return 0;
}