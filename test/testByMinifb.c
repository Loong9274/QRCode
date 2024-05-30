
#include "QRCode.h"
#include "MiniFB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static uint32_t g_width = 181;
static uint32_t *g_buffer = 0x0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void bitmap2UInt32(uint8_t* bitmap,uint8_t width,uint16_t len,uint32_t* intmap,uint16_t stride,uint32_t color)
{
    uint16_t count = 0;
    while (count<len)
    {
        if (bitmap[count/8]&(1<<(count%8)))
        {
            intmap[count/width*stride+(count%width)] = color;
        }
        count++;
    }
};

int main()
{
    uint32_t i, noise, carry, seed = 0xbeef;

    QRCode *qr = QRCode_generate("HELLO WORLD",
                               11,
                               QRCODE_MODE_NOSPECIFY,
                               QRCODE_ECC_NOSPECIFY,
                               0,
                               QRCODE_MASK_NOSPECIFY);
    if (qr == NULL)
    {
        printf("qr return null");
        return 1;
    }

    uint8_t qrWidth = qr->version*4+17;
    g_width = qrWidth + 4;
    g_width = g_width>32?g_width:33; // TODO issue minifb windows width min 164

    // init g_buffer
    g_buffer = (uint32_t *)calloc(4, g_width * g_width);
    for (size_t j = 0; j < g_width; j++)
    {
        g_buffer[j] = 0x00FFFFFF;
        g_buffer[g_width * (g_width - 1) + j] = 0x00FFFFFF;
        g_buffer[g_width * j] = 0x00FFFFFF;
        g_buffer[g_width * j + (g_width - 1)] = 0x00FFFFFF;
    }
    bitmap2UInt32(qr->data,
    qrWidth,
    qrWidth*qrWidth,
    g_buffer+(g_width-qrWidth)/2*(g_width+1), // in the center
    g_width,
    0x00FFFFFF);

    struct mfb_window *window = mfb_open_ex("QRCode test", g_width*4, g_width*4, WF_ALWAYS_ON_TOP);
    if (!window)
        return 0;

    mfb_update_state state;
    do
    {

        state = mfb_update_ex(window, g_buffer, g_width, g_width);
        if (state != STATE_OK)
        {
            window = 0x0;
            break;
        }
    } while (mfb_wait_sync(window));

    return 0;
}
