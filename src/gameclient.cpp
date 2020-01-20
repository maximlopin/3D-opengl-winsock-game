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
#include "eclass.h"

#define SOCKET_ERR_MSG(msg); fprintf(stderr, "%s (%s) (error %i)\n", msg, __func__, WSAGetLastError());

#define SLEEP(t0, freq) \
        double t1 = glfwGetTime(); \
        double dt = (t1 - t0); \
        double sleep_s = (1 / freq) - dt; \
        long long sleep_ms = static_cast<long long>(sleep_s * 1000); \
        if (sleep_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)); \

static const double INPUT_FREQ = 0.5;

static World world;

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

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 0;

    if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr)) == INVALID_SOCKET)
    {
        SOCKET_ERR_MSG("Failed to bind socket");
        closesocket(sock);
        return 1;
    }

    sockaddr_in auth_addr = SERVER_AUTH_ADDR;

    char blank_buf[1] = { 0 };
    if (sendto(sock, blank_buf, 1, 0, reinterpret_cast<sockaddr*>(&auth_addr), sizeof(sockaddr)) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to connect (sendto)");
        return 1;
    }

    int8_t buf[MAX_PACKET_SIZE];

    if (recv(sock, reinterpret_cast<char*>(buf), sizeof(int32_t), 0) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to connect (recv)");
        return 1;
    }

    /*
        Auth packet structure

        ----------------------------------------------
         int32_t | ID of newly created Hero_e instance
        ----------------------------------------------
    */

    int32_t id = *reinterpret_cast<int32_t*>(buf);
    
    if (id == -1)
    {
        INFO("Server returned ID = -1");
        closesocket(sock);
        return 0;
    }

    Hero_e local_hero(id);

    INFO("Connected with ID " << id);

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

    while (true)
    {
        if (recv(sock, reinterpret_cast<char*>(buf), MAX_PACKET_SIZE, 0) == SOCKET_ERROR)
        {
            SOCKET_ERR_MSG("Failed to receive data");
            closesocket(sock);
            return 1;
        }

        /*
            Sync entity packet structure

            -------------------------------
             uint8_t |  N (number of entities)
            -------------------------------
            struct A | struct A {
                .    |     uint8_t eclass; // Member of EClass struct
                .    |     int32_t id;
                N    | };
            -------------------------------
              BYTES  | Data.
                .    | Each entity
                .    | is responsible for
                M    | consuming it.
            -------------------------------
        */

        int32_t num_ents = *reinterpret_cast<int32_t*>(buf);

        int32_t einfo_offset = sizeof(num_ents);
        int32_t edata_offset = sizeof(num_ents) + num_ents * sizeof(int32_t) * 2;

        for (int i = 0; i < num_ents; i++)
        {
            int32_t eclass = *reinterpret_cast<int32_t*>(buf + einfo_offset);
            einfo_offset += sizeof(eclass);

            int32_t id =  *reinterpret_cast<int32_t*>(buf + einfo_offset);
            einfo_offset += sizeof(id);

            // INFO("Entity (EClass = " << eclass << ", id = " << id << ")");

            switch (eclass)
            {
                case EClass::ECLASS_HERO:
                {
                    Hero_e* hero_ptr = world.m_heroes.by_id(id);

                    if (hero_ptr == nullptr)
                    {
                        Hero_e hero(id);
                        hero.consume_buffer(buf + edata_offset);
                        world.m_heroes.force_add(id, &hero);
                        edata_offset += hero.get_buf_len();
                    }
                    else
                    {
                        hero_ptr->consume_buffer(buf + edata_offset);
                        edata_offset += hero_ptr->get_buf_len();
                    }
                }
                break;

                case EClass::ECLASS_MONSTER:
                {
                    Monster_e* monster_ptr = world.m_monsters.by_id(id);
                    if (monster_ptr == nullptr)
                    {
                        Monster_e monster(id);
                        monster.consume_buffer(buf + edata_offset);
                        world.m_monsters.force_add(id, &monster);
                        edata_offset += monster.get_buf_len();
                    }
                    else
                    {
                        monster_ptr->consume_buffer(buf + edata_offset);
                        edata_offset += monster_ptr->get_buf_len();
                    }
                }
                break;

                case EClass::ECLASS_DROPPED_ITEM:
                {
                    DroppedItem_e* item_ptr = world.m_dropped_items.by_id(id);
                    if (item_ptr == nullptr)
                    {
                        DroppedItem_e item(id);
                        item.consume_buffer(buf + edata_offset);
                        world.m_dropped_items.force_add(id, &item);
                        edata_offset += item.get_buf_len();
                    }
                    else
                    {
                        item_ptr->consume_buffer(buf + edata_offset);
                        edata_offset += item_ptr->get_buf_len();
                    }
                }
                break;

                WARNING("Received invalid EClass");
            }
        }

        INFO("Received packet of size " << edata_offset << " (" << static_cast<int>(num_ents) << " entities)");
    }
    return 0;
}