#include "winsock2.h"
#include "addr.h"
#include "entity.h"
#include "world.h"
#include <thread>

#define MAX_BUFFER ((2 << 15) - 1)

#define SOCKET_ERR_MSG(msg); fprintf(stderr, "%s (%s) (error %i)\n", msg, __func__, WSAGetLastError());

#define SLEEP(last_t, freq) \
    double t = glfwGetTime(); \
    double dt = (t - last_t); \
    double sleep_s = (1 / freq) - dt; \
    long long sleep_ms = static_cast<long long>(sleep_s * 1000); \
    if (sleep_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)); \
    last_t = t;

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

static World world = { 0 };

/* Iterates through players and send them entity data */
void _main_data()
{
    static char buf[MAX_BUFFER];

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in addr = SERVER_DATA_ADDR;

    if (bind(sock, (sockaddr*) &addr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return;
    }

    DynamicEntity_e::set_socket(sock);

    double last_t = glfwGetTime();

    while (1)
    {
        for (int k = 0; k < world.num_players; k++)
        {
            Player* p = world.players[k];

            DynamicEntity_e::begin(&(p->addr));

            for (int i = 0; i < world.num_players; i++)
            {
                world.players[i]->hero->enqueue();
            }

            for (int i = 0; i < world.num_monsters; i++)
            {
                world.monsters[i]->enqueue();
            }

            for (int i = 0; i < world.num_dropped_items; i++)
            {
                world.dropped_items[i]->enqueue();
            }

            DynamicEntity_e::end();
        }        
        SLEEP(last_t, PACKETS_FREQ);
        printf("Sending a packet\n");
    }
}

/* Adds new players to world for new connections */
void _main_auth()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in name = SERVER_AUTH_ADDR;

    if (bind(sock, (sockaddr*) &name, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return;
    }

    if (listen(sock, 1) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to listen to socket");
        closesocket(sock);
        return;
    }

    while (1)
    {
        Player* p = new Player();
        
        int addrlen = sizeof(sockaddr);
        SOCKET conn = accept(sock, (sockaddr*) &(p->addr), &addrlen);

        if (conn == INVALID_SOCKET)
        {
            SOCKET_ERR_MSG("Failed to accept connection");
            closesocket(sock);
            return;
        }

        printf("Got a connection\n");

        /* Loads player + hero data from database */
        p->load_data();
        
        closesocket(conn);
    }
    closesocket(sock);
}

/* Receives input and sends it to the right player in world */
void _main_input()
{
    SOCKET sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in addr = SERVER_INPUT_ADDR;
    if (bind(sock, (sockaddr*) &addr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return;
    }

    char buf[MAX_BUFFER];

    while (1)
    {
        sockaddr_in from;
        int fromlen = sizeof(sockaddr);
        if (recvfrom(sock, buf, MAX_BUFFER, 0, (sockaddr*) &from, &fromlen) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to recieve data");
            closesocket(sock);
            return;
        }

        printf("Received input\n");
    }
}

void _main_game()
{
    double last_t = glfwGetTime();

    while(1)
    {

        SLEEP(last_t, TICK_FREQ);
        printf("Ticked\n");
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
    std::thread input_thread(_main_input);
    std::thread game_thread(_main_game);
    std::thread data_thread(_main_data);

    auth_thread.join();
    input_thread.join();
    game_thread.join();
    data_thread.join();

    return 0;
}