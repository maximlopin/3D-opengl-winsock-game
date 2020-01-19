#ifndef ADDR_H
#define ADDR_H

#include "winsock2.h"

#define SERVER_AUTH_ADDR _server_auth_sockaddr();
#define SERVER_INPUT_ADDR _server_input_sockaddr();

#define HOST "127.0.0.1"

inline struct sockaddr_in _server_auth_sockaddr()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(HOST);
    addr.sin_port = htons(25565);
    return addr;
}

inline struct sockaddr_in _server_input_sockaddr()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(HOST);
    addr.sin_port = htons(25566);
    return addr;
}

#endif