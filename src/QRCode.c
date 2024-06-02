#include "QRCode.h"
#include <stdlib.h>
#include <string.h>
static const uint8_t AlignmentPatternLocations[] = {
    26, 46, 66, // version14
    26, 48, 70,
    26, 50, 74,
    30, 54, 78,
    30, 56, 82,
    30, 58, 86,
    34, 62, 90,
    28, 50, 72, 94, // version21
    26, 50, 74, 98,
    30, 54, 78, 102,
    28, 54, 80, 106,
    32, 58, 84, 110,
    30, 58, 86, 114,
    34, 62, 90, 118,
    26, 50, 74, 98, 122, // version28
    30, 54, 78, 102, 126,
    26, 52, 78, 104, 130,
    30, 56, 82, 108, 134,
    34, 60, 86, 112, 138,
    30, 58, 86, 114, 142,
    34, 62, 90, 118, 146,
    30, 54, 78, 102, 126, 150, // version35
    24, 50, 76, 102, 128, 154,
    28, 54, 80, 106, 132, 158,
    32, 58, 84, 110, 136, 162,
    26, 54, 82, 110, 138, 166,
    30, 58, 86, 114, 142, 170};

typedef struct
{
    uint8_t *data;
    uint16_t index;
    uint16_t len;
} QRCode_Bitset;

// TODO
static QRCode_Error QRCode_analyse(QRCode *qr, const uint8_t *raw, uint16_t len)
{
    qr->version = 1;
    qr->ecc = QRCODE_ECC_M;
    qr->mode = QRCODE_MODE_ALPHANUMERIC;
    return QRCODE_OK;
};

// TODO
static QRCode_Error QRCode_encode(QRCode *qr, const uint8_t *raw, uint16_t len, QRCode_Bitset *dataSet)
{
    const uint8_t dataHELLO_WORLD[] ={
        32, 91, 11, 120, 209, 114, 220, 77, 67, 64, 236, 17, 236, 17, 236, 17,
        196,  35,  39,  119,  235,  215,  231,  226,  93,  23
    };
    dataSet->data = malloc(sizeof(dataHELLO_WORLD));
    memcpy(dataSet->data,dataHELLO_WORLD,sizeof(dataHELLO_WORLD));
    return QRCODE_OK;
};
/////////////////// place modules //////////////////
static void QRCode_setModule(uint8_t *data, uint8_t width, uint8_t x, uint8_t y)
{
    data[(y * width + x) / 8] |= 1 << ((y * width + x) % 8);
    return;
};
static void QRCode_clrModule(uint8_t *data, uint8_t width, uint8_t x, uint8_t y)
{
    data[(y * width + x) / 8] &= ~(1 << ((y * width + x) % 8));
    return;
};
static void QRCode_setRectangle(uint8_t *data, uint8_t width, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    for (uint8_t y = y1; y < y2; y++)
    {
        for (uint8_t x = x1; x < x2; x++)
        {
            QRCode_setModule(data, width, x, y);
        }
    }
    return;
};

static void QRCode_placeBoundaryAligmentPattern(uint8_t *data, uint8_t width, uint8_t x, uint8_t y)
{
    QRCode_setRectangle(data, width, x - 2, y - 2, x + 3, y + 3);
}
static void QRCode_clearBoundaryAligmentPattern(uint8_t *data, uint8_t width, uint8_t x, uint8_t y)
{
    QRCode_clrModule(data, width, x - 1, y - 1);
    QRCode_clrModule(data, width, x, y - 1);
    QRCode_clrModule(data, width, x + 1, y - 1);
    QRCode_clrModule(data, width, x - 1, y + 1);
    QRCode_clrModule(data, width, x, y + 1);
    QRCode_clrModule(data, width, x + 1, y + 1);
    QRCode_clrModule(data, width, x - 1, y);
    QRCode_clrModule(data, width, x + 1, y);
}
static void QRCode_placeAligmentPatterns(QRCode *qr, uint8_t isBoundary)
{
    uint8_t width = (qr->version * 4 + 17);
    uint8_t *alignPatPos;
    int8_t alignPatNum = 0;
    void (*maker)(uint8_t *, uint8_t, uint8_t, uint8_t);
    maker = QRCode_clearBoundaryAligmentPattern;
    if (isBoundary)
    {
        maker = QRCode_placeBoundaryAligmentPattern;
    }

    if (qr->version != 1)
    {
        alignPatNum = qr->version / 7 + 1;
    }
    if (qr->version < 7)
    {
        alignPatPos = (uint8_t *)malloc(1);
        alignPatPos[0] = width - 7;
    }
    else if (qr->version < 14)
    {
        alignPatPos = (uint8_t *)malloc(2);
        alignPatPos[1] = width - 7;
        alignPatPos[0] = (alignPatPos[1] + 6) / 2;
    }
    else
    {
        alignPatPos = (uint8_t *)AlignmentPatternLocations + (qr->version < 21 ? 0 : (qr->version < 28 ? 21 : (qr->version < 35 ? 49 : 84))) + alignPatNum * (qr->version % 7);
    }

    for (uint8_t i = 0; i < alignPatNum; i++)
    {
        for (uint8_t j = 0; j < alignPatNum; j++)
        {
            maker(qr->data, width, alignPatPos[i], alignPatPos[j]);
        }
    }
    for (int8_t i = 0; i < alignPatNum - 1; i++)
    {
        maker(qr->data, width, 6, alignPatPos[i]);
        maker(qr->data, width, alignPatPos[i], 6);
    }
}
static void QRCode_placeBoundary(QRCode *qr)
{
    uint8_t width = (qr->version * 4 + 17);
    // finder pattern
    QRCode_setRectangle(qr->data, width, 0, 0, 9, 9);
    QRCode_setRectangle(qr->data, width, width - 8, 0, width, 9);
    QRCode_setRectangle(qr->data, width, 0, width - 8, 9, width);
    // dark module
    QRCode_setModule(qr->data, width, 8, width - 8);
    // alignment pattern
    QRCode_placeAligmentPatterns(qr, 1);
    // timing pattern
    for (uint8_t i = 9; i < width - 8; i++)
    {
        QRCode_setModule(qr->data, width, i, 6);
        QRCode_setModule(qr->data, width, 6, i);
    }
    // version info area
    if (qr->version > 6)
    {
        QRCode_setRectangle(qr->data, width, width - 11, 0, width - 8, 6);
        QRCode_setRectangle(qr->data, width, 0, width - 11, 6, width - 8);
    }
    return;
};
static void QRCode_placeDataModule(uint8_t *bitmap, uint8_t width, uint8_t x, uint8_t y, uint8_t *data, uint32_t *count)
{
    if (!(bitmap[(y * width + x) / 8] & (1 << ((y * width + x) % 8))))
    {
        if (((data[(*count) / 8] & (0x80 >> ((*count) % 8))) != 0) ^ (((x + y) % 2) == 0))
        {
            bitmap[(y * width + x) / 8] |= (1 << ((y * width + x) % 8));
        }
        (*count)++;
    }
};
static void QRCode_placeDataModules(QRCode *qr, uint8_t *data)
{
    int16_t width = qr->version * 4 + 17;
    int16_t x = width - 1;
    int16_t y = width - 1;
    uint32_t count = 0;
    for (size_t i = 0; i < 26; i++)
    {
        printf("%d, ",data[i]);
    }printf("\n");
    
    while (x > 1)
    {
        while (y >= 0)
        {
            QRCode_placeDataModule(qr->data,width,x,y,data,&count);
            x--;
            QRCode_placeDataModule(qr->data,width,x,y,data,&count);
            x++;
            y--;
        }
        y++;
        if (x == 8)
        {
            x--;
        }
        x -= 2;
        while (y < width)
        {
            QRCode_placeDataModule(qr->data,width,x,y,data,&count);
            x--;
            QRCode_placeDataModule(qr->data,width,x,y,data,&count);
            x++;
            y++;
        }
        y--;
        x -= 2;
    }
    printf("count: %d\n",count);
    return;
};

static void QRCode_clearPattern(QRCode *qr)
{
    uint8_t width = qr->version * 4 + 17;
    for (uint8_t i = 0; i < 5; i++)
    {
        QRCode_clrModule(qr->data, width, 1 + i, 1);
        QRCode_clrModule(qr->data, width, 1 + i, 5);
        QRCode_clrModule(qr->data, width, width - 6 + i, 1);
        QRCode_clrModule(qr->data, width, width - 6 + i, 5);
        QRCode_clrModule(qr->data, width, 1 + i, width - 6);
        QRCode_clrModule(qr->data, width, 1 + i, width - 2);
    }
    for (uint8_t i = 0; i < 3; i++)
    {
        QRCode_clrModule(qr->data, width, 1, 2 + i);
        QRCode_clrModule(qr->data, width, 5, 2 + i);
        QRCode_clrModule(qr->data, width, width - 6, 2 + i);
        QRCode_clrModule(qr->data, width, width - 2, 2 + i);
        QRCode_clrModule(qr->data, width, 1, width - 5 + i);
        QRCode_clrModule(qr->data, width, 5, width - 5 + i);
    }
    for (uint8_t i = 0; i < 8; i++)
    {
        QRCode_clrModule(qr->data, width, i, 7);
        QRCode_clrModule(qr->data, width, 7, i);
        QRCode_clrModule(qr->data, width, width - 8, i);
        QRCode_clrModule(qr->data, width, width - 8 + i, 7);
        QRCode_clrModule(qr->data, width, i, width - 8);
        QRCode_clrModule(qr->data, width, 7, width - 8 + i);
    }
    for (uint8_t i = 9; i < width - 9; i += 2)
    {
        QRCode_clrModule(qr->data, width, i, 6);
        QRCode_clrModule(qr->data, width, 6, i);
    }
    QRCode_placeAligmentPatterns(qr, 0);
    return;
};

static void QRCode_placeInfoModules(QRCode *qr)
{
    uint16_t data = 0x5412;
    uint8_t width = qr->version*4+17;
    for (size_t i = 0; i < 6; i++)
    {
        if (!(data&(0x4000>>i)))
        {
            QRCode_clrModule(qr->data,width,i,8);
            QRCode_clrModule(qr->data,width,8,width-1-i);
        }   
    }
    if (!(data&(0x4000>>6)))
    {
        QRCode_clrModule(qr->data,width,7,8);
        QRCode_clrModule(qr->data,width,8,width-7);
    }
    if (!(data&(0x4000>>7)))
    {
        QRCode_clrModule(qr->data,width,8,8);
        QRCode_clrModule(qr->data,width,width-8,8);
    }
    if (!(data&(0x4000>>8)))
    {
        QRCode_clrModule(qr->data,width,8,7);
        QRCode_clrModule(qr->data,width,width-7,8);
    }
    for (size_t i = 9; i < 15; i++)
    {
        if (!(data&(0x4000>>i)))
        {
            QRCode_clrModule(qr->data,width,width-15+i,8);
            QRCode_clrModule(qr->data,width,8,14-i);
        }   
    }
    
}
static void QRCode_placeInfoAndPattern(QRCode *qr)
{
    QRCode_placeInfoModules(qr);
    QRCode_clearPattern(qr);
    return;
};

// TODO
static QRCode_Error QRCode_placeModules(QRCode *qr, uint8_t *data)
{
    uint16_t bufferSize = ((qr->version * 4 + 17) * (qr->version * 4 + 17) + 7) / 8;
    qr->data = calloc(bufferSize, sizeof(uint8_t));
    // for (size_t i = 0; i < bufferSize; i++)
    // {
    //     qr->data[i] = 0xAA;
    // }
    QRCode_placeBoundary(qr);
    QRCode_placeDataModules(qr, data);
    QRCode_placeInfoAndPattern(qr);
    return QRCODE_OK;
};
/////////////////// place modules end //////////////////

QRCode *QRCode_generate(const uint8_t *raw,
                        uint16_t len,
                        QRCode_Mode mode,
                        QRCode_Ecc ecc,
                        uint8_t version,
                        QRCode_Mask mask)
{
    QRCode_Bitset dataSet;
    QRCode *qr = (QRCode *)calloc(1, sizeof(QRCode));
    qr->mode = mode;
    qr->ecc = ecc;
    qr->version = version;
    qr->mask = mask;
    // check mode, version
    if (QRCode_analyse(qr, raw, len) != QRCODE_OK)
    {
        return NULL;
    }
    if (QRCode_encode(qr, raw, len, &dataSet) != QRCODE_OK)
    {
        return NULL;
    }
    if (QRCode_placeModules(qr, dataSet.data) != QRCODE_OK)
    {
        return NULL;
    }
    else
    {
        return qr;
    }
};

#include <stdio.h>
void testQRCode(void)
{
    printf("Hello QRCode\n");
    return;
}
