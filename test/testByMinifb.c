
#include "QRCode.h"
#include "MiniFB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static const uint32_t  g_width  = 720;
static uint32_t *g_buffer = 0x0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int 
main()
{
    uint32_t    i, noise, carry, seed = 0xbeef;

    struct mfb_window *window = mfb_open_ex("QRCode test", g_width, g_width, WF_ALWAYS_ON_TOP);
    if (!window)
        return 0;

    // init g_buffer
    g_buffer = (uint32_t *) calloc(4, g_width * g_width);
    for (size_t j = 3; j < g_width-3; j++)
    {
        g_buffer[j] = 0x00FFFFFF;
        g_buffer[g_width*(g_width-1)+j] = 0x00FFFFFF;
        g_buffer[g_width*j] = 0x00FFFFFF;
        g_buffer[g_width*j+(g_width-1)] = 0x00FFFFFF;

        g_buffer[j+g_width*2] = 0x00FFFFFF;
        g_buffer[g_width*(g_width-3)+j] = 0x00FFFFFF;
        g_buffer[g_width*j+2] = 0x00FFFFFF;
        g_buffer[g_width*j+(g_width-3)] = 0x00FFFFFF;

        g_buffer[j+g_width*3] = 0x00FFFFFF;
        g_buffer[g_width*(g_width-4)+j] = 0x00FFFFFF;
        g_buffer[g_width*j+3] = 0x00FFFFFF;
        g_buffer[g_width*j+(g_width-4)] = 0x00FFFFFF;
    }

    testQRCode();
    mfb_update_state state;
    do {
        

        state = mfb_update_ex(window, g_buffer, g_width, g_width);
        if (state != STATE_OK) {
            window = 0x0;
            break;
        }
    } while(mfb_wait_sync(window));

    return 0;
}


