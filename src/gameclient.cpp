#include "winsock2.h"
#include "math.h"
#include "addr.h"
#include "input.h"
#include "world.h"
#include <iostream>
#include <thread>
#include <mutex>
#include "auth.h"
#include "logging.h"

#define SOCKET_ERR_MSG(msg); fprintf(stderr, "%s (%s) (error %i)\n", msg, __func__, WSAGetLastError());

#define SLEEP(t0, freq) \
        double t1 = glfwGetTime(); \
        double dt = (t1 - t0); \
        double sleep_s = (1 / freq) - dt; \
        long long sleep_ms = static_cast<long long>(sleep_s * 1000); \
        if (sleep_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)); \

static const double INPUT_FREQ = 0.5;

volatile static World world;

volatile static Hero_e local_hero;

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

    while (true)
    {
        double t0 = glfwGetTime();

        if (sendto(sock, msg, 6, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to send data");
            return;
        }

        SLEEP(t0, INPUT_FREQ)

    }
}

/*
    Sync entity packet structure

    -------------------------------
     uint8_t |  N (number of entities)
    -------------------------------
    struct A | struct A {
       .     |     uint8_t eclass; // Member of EClass struct
       .     |     int32_t id; // ID is unique among *this* class of entities
       N     | };
    -------------------------------
      BYTES  | Data.
        .    | Each entity
        .    | is responsible for
        M    | consuming it.
    -------------------------------
*/

void _main_data()
{
    // SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    // if (sock == INVALID_SOCKET)
    // {
    //     SOCKET_ERR_MSG("Failed to create socket");
    //     return;
    // }

    // sockaddr_in addr = CLIENT_DATA_ADDR;

    // if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == SOCKET_ERROR)
    // {
    //     SOCKET_ERR_MSG("Failed to bind socket");
    //     closesocket(sock);
    //     return;
    // }

    // char buf[MAX_PACKET_SIZE];

    // while (true)
    // {
    //     if (recv(sock, buf, MAX_PACKET_SIZE, 0) == SOCKET_ERROR)
    //     {
    //         SOCKET_ERR_MSG("Failed to receive data");
    //         closesocket(sock);
    //         return;
    //     }

    //     uint8_t num_ents = ((uint8_t*) buf)[0];
    //     uint16_t data_offset = sizeof(uint8_t) + (sizeof(uint8_t) + sizeof(int32_t)) * num_ents;

    //     uint16_t e_offset = 0;
    //     for (int i = 0; i < num_ents; i++)
    //     {
    //         uint8_t eclass = (uint8_t) (buf + data_offset + (sizeof(uint8_t) + sizeof(int32_t)) * i);
    //         int32_t id = (int32_t) (buf + data_offset + (sizeof(uint8_t) + sizeof(int32_t)) * i + sizeof(uint8_t));
    //         switch (eclass)
    //         {
    //             case ECLASS_HERO:
    //                 world.m_heroes
    //                 break;

    //             case ECLASS_LOCAL_HERO:
    //                 world.m_heroes
    //                 world.m_heroes.by_id(local_hero_id)
    //                 break;

    //             case ECLASS_MONSTER:
    //                 world.m_monsters
    //                 break;

    //             case ECLASS_DROPPED_ITEM:
    //                 world.m_dropped_items
    //                 break;

    //             default:
    //                 WARNING("Received invalid ECLASS");
    //                 break;
    //         }
    //         e.consume_buffer((buf + data_offset + e_offset));
    //         e_offset += e.get_buf_len();
    //     }
    // }
}

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "Failed to initialize WSA\n");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to create socket");
        return 1;
    }

    sockaddr_in auth_addr = SERVER_AUTH_ADDR;

    char buf[MAX_BUF_SIZE];

    if (sendto(sock, buf, MAX_BUF_SIZE, 0, reinterpret_cast<sockaddr*>(&auth_addr), sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to connect");
        return 1;
    }

    recv(sock, buf, MAX_BUF_SIZE, 0);



    INFO("Connected");

    GLFWwindow* window = make_window(1280, 720, "My window");

    glfwSetWindowUserPointer(window, (void*) &local_hero.m_input);

    glfwSetWindowSizeCallback(window, [](GLFWwindow* wnd, int w, int h) {
        glViewport(0, 0, w, h);
        Model::update_perspective(w, h);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double x, double y) {
        int w, h;
        glfwGetWindowSize(wnd, &w, &h);

        double cx = static_cast<double>(w) / 2;
        double cy = static_cast<double>(h) / 2;

        Input* p_input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(wnd));
        p_input->cursor_theta = atan2l((x - cx), (y - cy));
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int key, int action, int mods) {
        bool state;

        Input* p_input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(wnd));

        if (action == GLFW_PRESS)
            state = true;
        else if (action == GLFW_RELEASE)
            state = false;
        else
            return;
        
        switch (key)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                p_input->LM_PRESSED = state;
                break;

            case GLFW_MOUSE_BUTTON_RIGHT:
                p_input->RM_PRESSED = state;
                break;

            default:
                break;
        }
    });

    std::thread input_thread(_main_input);

    input_thread.join();

    return 0;
}