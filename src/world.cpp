#include "world.h"

static int cmp_addr(sockaddr_in* a, sockaddr_in* b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) && (a->sin_port == b->sin_port);
}

World::World()
{

}