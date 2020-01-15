#include "input.h"

void hton_input(Input* in, Input* out)
{
    // out->cursor_theta = htond(in->cursor_theta);
    out->LM_PRESSED = htonl(in->LM_PRESSED);
    out->RM_PRESSED = htonl(in->RM_PRESSED);
}

void ntoh_input(Input* in, Input* out)
{
    // out->cursor_theta = ntohd(in->cursor_theta);
    out->LM_PRESSED = ntohl(in->LM_PRESSED);
    out->RM_PRESSED = ntohl(in->RM_PRESSED);
}