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
        if (sleep_s > 0) std::this_thread::sleep_for(std::chrono::seconds(static_cast<long long>(sleep_s))); \

static const double PACKETS_FREQ = 1.0;
static const double TICK_FREQ = 20.0;

volatile bool running = true;

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

    while (running)
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

/* Receives input from players and applies it to corresponding heroes */
void _main_input()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in addr = SERVER_INPUT_ADDR

    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return;
    }

    int8_t buf[MAX_PACKET_SIZE];

    while (true)
    {
        sockaddr_in from;
        int fromlen = sizeof(sockaddr);

        if (recvfrom(sock, reinterpret_cast<char*>(buf), MAX_PACKET_SIZE, 0, reinterpret_cast<sockaddr*>(&from), &fromlen) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to receive data");
            closesocket(sock);
            return;
        }

        std::string key = addr_to_string(from);

        if (Player::s_players.find(key) != Player::s_players.end())
        {
            int32_t hero_id = Player::s_players[key]->m_hero_id;
            Hero_e* hero_ptr = world.m_heroes.by_id(hero_id);
            hero_ptr->consume_input_buffer(buf);

            INFO("Received input from " << key << " (success): " << hero_ptr->m_input.cursor_theta << ", " << hero_ptr->m_input.LM_PRESSED << ", " << hero_ptr->m_input.RM_PRESSED);
        }
        else
        {
            INFO("Received input from " << key << " (fail)");
        }
    }
}

void _main_game()
{
    double last_t = glfwGetTime();
    while (running)
    {
        double t = glfwGetTime();
        double dt = t - last_t;
        last_t = t;

        for (int i = 0; i < world.m_heroes.size(); i++)
        {
            world.m_heroes.by_index(i)->tick(dt);
        }

        INFO("Ticked");

        double sleep_s = (1 / TICK_FREQ) - dt;
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<long long>(sleep_s)));
    }
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
    std::thread input_thread(_main_input);
    std::thread game_thread(_main_game);

    auth_thread.join();
    data_thread.join();
    input_thread.join();
    game_thread.join();

    for (auto keyval : Player::s_players)
    {
        Player* p_player = keyval.second;
        delete p_player;
    }

    return 0;
}