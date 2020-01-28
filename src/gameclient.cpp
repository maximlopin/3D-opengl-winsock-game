#include "winsock2.h"
#include "math.h"
#include "addr.h"
#include "input.h"
#include "world.h"
#include <iostream>
#include <thread>
#include "auth.h"
#include "logging.h"
#include "eclass.h"

#define SOCKET_ERR_MSG(msg); fprintf(stderr, "%s (%s) (error %i)\n", msg, __func__, WSAGetLastError());

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
        closesocket(sock);
        return 1;
    }

    int8_t buf[MAX_PACKET_SIZE];

    if (recv(sock, reinterpret_cast<char*>(buf), sizeof(int32_t), 0) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to connect (recv)");
        closesocket(sock);
        return 1;
    }

    int32_t id = *reinterpret_cast<int32_t*>(buf);
    
    if (id == -1)
    {
        INFO("Server returned ID = -1");
        closesocket(sock);
        return 1;
    }

    INFO("Connected with ID " << id);

    u_long mode;
    if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR)
    {
        SOCKET_ERR_MSG("Failed to set non-blocking mode");
        closesocket(sock);
        return 1;
    }

    GLFWwindow* window = make_window(1280, 720, "My window");
    Model::init(1280, 720);

    World world;

    glfwSetWindowSizeCallback(window, [](GLFWwindow* wnd, int w, int h) {
        glViewport(0, 0, w, h);
        Model::update_perspective(w, h);
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* wnd) {
        glfwSetWindowShouldClose(wnd, 1);
    });

    Hero_e local_hero(id);
    world.m_heroes.set(id, &local_hero);

    sockaddr_in input_addr = SERVER_INPUT_ADDR;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Hero_e* local_hero_ptr = world.m_heroes.by_index(0);

        vec3 origin;
        origin[0] = local_hero_ptr->m_pos.pos[0];
        origin[1] = local_hero_ptr->m_pos.pos[1];
        origin[2] = local_hero_ptr->m_pos.elevation;

        vec3 world_pos = { 0.0, 0.0, 0.0 };
        world.m_mesh.render(origin, world_pos);

        for (int i = 0; i < world.m_heroes.size(); i++)
        {
            Hero_e* hero_ptr = world.m_heroes.by_index(i);

            vec3 pos;
            pos[0] = hero_ptr->m_pos.pos[0];
            pos[1] = hero_ptr->m_pos.pos[1];
            pos[2] = hero_ptr->m_pos.elevation;

            hero_ptr->m_mesh.render(origin, pos);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    
        if (recv(sock, reinterpret_cast<char*>(buf), MAX_PACKET_SIZE, 0) == SOCKET_ERROR)
        {
            int error = WSAGetLastError();

            if (error == WSAEWOULDBLOCK)
            {
                // Do nothing
            }

            else if (error == WSAEMSGSIZE)
            {
                INFO("Received packet exceeding MAX_PACKET_SIZE");
            }

            else
            {
                SOCKET_ERR_MSG("Error while receiving data");
                closesocket(sock);
                break;
            }
        }
        else
        {
            int32_t num_ents = *reinterpret_cast<int32_t*>(buf);

            int32_t einfo_offset = sizeof(num_ents);
            int32_t edata_offset = sizeof(num_ents) + num_ents * sizeof(int32_t) * 2;

            for (int i = 0; i < num_ents; i++)
            {
                int32_t eclass = *reinterpret_cast<int32_t*>(buf + einfo_offset);
                einfo_offset += sizeof(eclass);

                int32_t id =  *reinterpret_cast<int32_t*>(buf + einfo_offset);
                einfo_offset += sizeof(id);

                Sync_s* sync_ptr = nullptr;

                switch (eclass)
                {
                    case EClass::ECLASS_HERO:
                    {
                        if ((sync_ptr = static_cast<Sync_s*>(world.m_heroes.by_id(id))) == nullptr)
                        {
                            Hero_e* hero_ptr = world.m_heroes.allocate(id);
                            new (hero_ptr) Hero_e(id);
                            sync_ptr = static_cast<Sync_s*>(hero_ptr);
                        }
                    }
                    break;
                    WARNING("Received invalid EClass");
                }

                if (sync_ptr != nullptr)
                {
                    sync_ptr->consume_buffer(buf + edata_offset);
                    edata_offset += sync_ptr->get_buf_len();
                }
            }

            Hero_e* hero_ptr = world.m_heroes.by_index(0);
            int32_t len = hero_ptr->fill_input_buffer(buf);

            int w, h;
            glfwGetWindowSize(window, &w, &h);

            double x, y;
            glfwGetCursorPos(window, &x, &y);

            double cx = static_cast<double>(w) / 2;
            double cy = static_cast<double>(h) / 2;
            
            hero_ptr->m_input.cursor_theta = atan2l((x - cx), (y - cy));

            hero_ptr->m_input.LM_PRESSED = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            hero_ptr->m_input.RM_PRESSED = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

            if (sendto(sock, reinterpret_cast<char*>(buf), len, 0, reinterpret_cast<sockaddr*>(&input_addr), sizeof(sockaddr)) == SOCKET_ERROR)
            {
                SOCKET_ERR_MSG("Failed to send");
                closesocket(sock);
                break;
            }
        }
    }

    WSACleanup();
    glfwTerminate();

    return 0;
}