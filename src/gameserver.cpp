#include "winsock2.h"
#include "addr.h"
#include "userlist.h"
#include "entity.h"
#include "world.h"
#include <thread>

#define MAX_BUFFER ((2 << 15) - 1)

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

static const int PACKETS_FREQ = 20;

static World world = { 0 };

/* Iterates through players and send them entity data */
void _main_data()
{
    static char buf[MAX_BUFFER];

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to create socket in %s\n", __func__);
        return;
    }

    struct sockaddr_in addr = CLIENT_DATA_ADDR;

    if (bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to bind socket in %s\n", __func__);
        closesocket(sock);
        return;
    }

    DynamicEntity_e::set_socket(sock);

    while (1)
    {
        double last_t = glfwGetTime();
    
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

            double t = glfwGetTime();
            double dt = t - last_t;
            double sleep_t = (1 / PACKETS_FREQ) - dt;
            if (sleep_t > 0.0)
            {
                Sleep((DWORD) (sleep_t * 1000));
            }
            last_t = t;
        }        
    }
}

/* Adds new players to world for new connections */
void _main_auth()
{
    SOCKET sock;
    if (sock = socket(sock, AF_INET, SOCK_STREAM) == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to create socket in %s\n", __func__);
        exit(1);
    }

    struct sockaddr_in name = SERVER_AUTH_ADDR;

    if (bind(sock, (struct sockaddr*) &name, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to bind socket in %s\n", __func__);
        closesocket(sock);
        exit(1);
    }

    if (listen(sock, 1) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to listen to socket in %s\n", __func__);
        closesocket(sock);
        exit(1);
    }

    while (1)
    {
        SOCKET conn;

        Player* p = new Player();
        int addrlen;

        if (accept(conn, (struct sockaddr*) &(p->addr), &addrlen) == SOCKET_ERROR)
        {
            fprintf(stderr, "Failed to accept connection in %s\n", __func__);
            closesocket(sock);
            exit(1);
        }

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
        fprintf(stderr, "Failed to create socket in %s\n", __func__);
        return;
    }

    struct sockaddr_in addr = SERVER_INPUT_ADDR;
    if (bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to bind socket in %s\n", __func__);
        closesocket(sock);
        return;
    }

    const int len = MAX_BUFFER;
    char buf[MAX_BUFFER];

    while (1)
    {
        struct sockaddr_in from;
        int fromlen;
        if (recvfrom(sock, buf, len, 0, (struct sockaddr*) &from, &fromlen) == SOCKET_ERROR)
        {
            fprintf(stderr, "Failed to recieve data in %s\n", __func__);
            closesocket(sock);
            return;
        }
    }
}

void _main_game()
{
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
    std::thread data_thread(_main_game);

    auth_thread.join();
    input_thread.join();
    game_thread.join();
    data_thread.join();

    return 0;
}