#include "winsock2.h"
#include "addr.h"
#include "world.h"
#include <iostream>
#include <thread>
#include <mutex>
#include "logging.h"

#define SOCKET_ERR_MSG(msg); fprintf(stderr, "%s (%s) (error %i)\n", msg, __func__, WSAGetLastError());

/* Packets are sent with this frequency as well */
const double TICK_FREQ = 30;

volatile bool running = true;

World world;
std::mutex world_mutex;

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

    while (running)
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

        world_mutex.lock();
        Player* player_ptr = Player::create(from, world);

        int32_t id = player_ptr == nullptr ? -1 : player_ptr->m_hero_id;
        world_mutex.unlock();
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

    while (running)
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
        world_mutex.lock();
        if (Player::s_players.find(key) != Player::s_players.end())
        {
            int32_t hero_id = Player::s_players[key]->m_hero_id;
            Hero_e* hero_ptr = world.m_heroes.by_id(hero_id);
            hero_ptr->consume_input_buffer(buf);

            // INFO("Received input from " << key << " (success): " << hero_ptr->m_input.cursor_theta << ", " << hero_ptr->m_input.LM_PRESSED << ", " << hero_ptr->m_input.RM_PRESSED);
        }
        else
        {
            WARNING("Received input from an unconnected address");
        }
        world_mutex.unlock();
    }
}

void _main_game()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    double dt = 0.0;

    while (running)
    {
        double t0 = glfwGetTime();

        world_mutex.lock();

        /* Tick */
        for (int i = 0; i < world.m_heroes.size(); i++)
        {
            world.m_heroes.by_index(i)->tick(dt);
        }

        /* Send packets */
        Player::distribute_packets(sock);

        world_mutex.unlock();    

        long long sleep_ms = static_cast<long long>(1000 * (1.0 / TICK_FREQ - dt));
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        dt = glfwGetTime() - t0;
    }
}

int main()
{
    glfwInit();

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Failed to initialize WSA\n");
        return 1;
    }

    std::thread auth_thread(_main_auth);
    std::thread input_thread(_main_input);
    std::thread game_thread(_main_game);

    auth_thread.join();
    input_thread.join();
    game_thread.join();

    for (auto keyval : Player::s_players)
    {
        Player* p_player = keyval.second;
        delete p_player;
    }

    return 0;
}