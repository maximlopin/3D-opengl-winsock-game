#include "entity.h"
#include "winsock2.h"
#include "addr.h"
#include <windows.h>

#define MAX_BUFFER ((2 << 15) - 1)

void _main_auth()
{

}

void _main_input()
{

}

void _main_game()
{
}

int main()
{
    _main_auth();
    
    SOCKET sock;
    if (sock = socket(sock, AF_INET, SOCK_STREAM) == INVALID_SOCKET)
    {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }
    
    struct sockaddr_in addr = SERVER_AUTH_ADDR;

    if (connect(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Failed to connect\n");
        exit(1);
    }

    char buf[MAX_BUFFER];

    if (recv(sock, buf, MAX_BUFFER, 0) == SOCKET_ERROR)
    {
        fpritnf(stderr, "Failed to receive\n");
        exit(1);
    }



    return 0;
}