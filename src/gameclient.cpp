#include "winsock2.h"
#include "entity.h"
#include "addr.h"
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

static const double INPUT_FREQ = 1.0;

void _main_input()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in addr = SERVER_INPUT_ADDR;

    char msg[] = { 'h', 'e', 'l', 'l', 'o', 0 };

    double last_t = glfwGetTime();

    while (true)
    {
        if (sendto(sock, msg, 6, 0, (sockaddr*) &addr, sizeof(sockaddr)) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to send data");
        }

        SLEEP(last_t, INPUT_FREQ);
    }
}

void _main_data()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return;
    }

    sockaddr_in addr = CLIENT_DATA_ADDR;

    if (bind(sock, (sockaddr*) &addr, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return;
    }

    char buf[MAX_BUFFER];

    while (true)
    {
        if (recv(sock, buf, MAX_BUFFER, 0) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to receive data");
            closesocket(sock);
            return;
        }
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

    std::thread input_thread(_main_input);
    std::thread data_thread(_main_data);

    input_thread.join();
    data_thread.join();

    return 0;
}