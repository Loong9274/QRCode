#include "QRCode.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define PRINT_ARRAY(array) do{printf(#array": [");for(size_t i=0;i<sizeof(array);i++){printf("%d, ",array[i]);}printf("]\n");}while (0)
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

typedef uint8_t GF256;
static const GF256 toGF256[256]={
    0,0,1,25,2,50,26,198,3,223,51,238,27,104,199,75,4,100,224,14,52,
141,239,129,28,193,105,248,200,8,76,113,5,138,101,47,225,36,15,33,53,
147,142,218,240,18,130,69,29,181,194,125,106,39,249,185,201,154,9,120,77,
228,114,166,6,191,139,98,102,221,48,253,226,152,37,179,16,145,34,136,54,
208,148,206,143,150,219,189,241,210,19,92,131,56,70,64,30,66,182,163,195,
72,126,110,107,58,40,84,250,133,186,61,202,94,155,159,10,21,121,43,78,
212,229,172,115,243,167,87,7,112,192,247,140,128,99,13,103,74,222,237,49,
197,254,24,227,165,153,119,38,184,180,124,17,68,146,217,35,32,137,46,55,
63,209,91,149,188,207,205,144,135,151,178,220,252,190,97,242,86,211,171,20,
42,93,158,132,60,57,83,71,109,65,162,31,45,67,216,183,123,164,118,196,
23,73,236,127,12,111,246,108,161,59,82,41,157,85,170,251,96,134,177,187,
204,62,90,203,89,95,176,156,169,160,81,11,245,22,235,122,117,44,215,79,
174,213,233,230,231,173,232,116,214,244,234,168,80,88,175
};
static const uint8_t toUInt8[256]={
    1,2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38, 76, 152, 45, 90, 180,
117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192, 157, 39, 78, 156, 37, 74, 148, 53, 106,
212, 181, 119, 238, 193, 159, 35, 70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185,
111, 222, 161, 95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240, 253,
231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226, 217, 175, 67, 134, 17,
34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206, 129, 31, 62, 124, 248, 237, 199, 147, 59,
118, 236, 197, 151, 51, 102, 204, 133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132,
21, 42, 84, 168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115, 230,
209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241,255, 227, 219, 171, 75, 150,
49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65, 130, 25, 50, 100, 200, 141, 7, 14, 28,
56, 112, 224, 221, 167, 83, 166, 81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172,
69, 138, 9, 18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22, 44,
88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142,1
};

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

static void QRCode_appendBitset(uint8_t* bitset,uint16_t bitIndex,uint32_t data,uint8_t bitNum)
{
    size_t remUInt8 = 8 -(bitIndex % 8);
    size_t byteIndex = bitIndex/8;

    //对齐边界
    if (remUInt8)
    {
        if (bitNum>remUInt8)
        {
            bitset[byteIndex] |=  ((data&(~(0x000000FF<<bitNum))) >> (bitNum-remUInt8));
            bitNum -= remUInt8;
            byteIndex++;
        }
        else
        {
            bitset[byteIndex] |=  ((data&(~(0x000000FF<<bitNum))) << (remUInt8-bitNum));
            bitNum = 0;
        }
    }
    while (bitNum >= 8)
    {
        bitNum -= 8;
        bitset[byteIndex++] = data >> bitNum;
    }
    if (bitNum)
    {
        bitset[byteIndex] = data << (8-bitNum);
    }
    return;
};

static void print_byte(uint8_t data)
{
    for (uint8_t i = 0x80; i; i >>= 1)
    {
        printf("%d", (i & data) != 0);
    }
    printf(" ");
};
static void print_binary(uint8_t *data, uint32_t len)
{
#define BYTE_IN_LINE 8
    size_t i = 0;
    for (; i+BYTE_IN_LINE <= len; i += BYTE_IN_LINE)
    {
        printf("%4d  ", i);
        for (size_t j = 0; j < BYTE_IN_LINE; j++)
        {
            print_byte(data[i + j]);
        }
        printf("\n");
    }
    if ((len%BYTE_IN_LINE) != 0)
    {
        printf("%4d  ", i);
        for (; i<len;i++)
        {
            print_byte(data[i]);
        }
        printf("\n");
    }
};

uint8_t QRCode_ALPHANUMERIC_toUInt8(uint8_t data)
{
    if ((data >= 'A') && (data <= 'Z'))
        return data - 'A' + 10;
    else if ((data >= '0') && (data <= '9'))
        return data - '0';
    else
    {
        switch (data)
        {
        case ' ':
            return 36;
        case '$':
            return 37;
        case '%':
            return 38;
        case '*':
            return 39;
        case '+':
            return 40;
        case '-':
            return 41;
        case '.':
            return 42;
        case '/':
            return 43;
        case ':':
            return 44;
        }
    }
    return 0;
}
static void QRCode_genDataBytes_ALPHANUMERIC(const uint8_t *raw, uint16_t rawLen,uint8_t *dataBytes,uint8_t dataLen)
{
    uint16_t bitIndex = 0;
    QRCode_appendBitset(dataBytes,bitIndex,0x02,4);
    bitIndex+=4;
    QRCode_appendBitset(dataBytes,bitIndex,rawLen,9);
    bitIndex+=9;
    for (size_t i = 0; i < rawLen-2; i+=2)
    {
        uint16_t temp = QRCode_ALPHANUMERIC_toUInt8(raw[i])*45+QRCode_ALPHANUMERIC_toUInt8(raw[i+1]);
        QRCode_appendBitset(dataBytes,bitIndex,temp,11);
        bitIndex+=11;
    }
    if ((rawLen%2)==1)
    {
        uint16_t temp = QRCode_ALPHANUMERIC_toUInt8(raw[rawLen-1]);
        QRCode_appendBitset(dataBytes,bitIndex,temp,6);
        bitIndex+=5;
    }
    print_binary(dataBytes,16);
    
    bitIndex += (dataLen*8-bitIndex)>4?4:(dataLen*8-bitIndex);
    bitIndex = (bitIndex+7)/8*8;
    while ((bitIndex+16)<=(dataLen*8))
    {
        QRCode_appendBitset(dataBytes,bitIndex,0xEC11,16);
        bitIndex+=16;
    }
    if (bitIndex<dataLen*8)
    {
        QRCode_appendBitset(dataBytes,bitIndex,0xEC,8);
        bitIndex+=8;
    }
    

};

static void QRCode_genDataBytes(QRCode *qr, const uint8_t *raw, uint16_t len,uint8_t *dataBytes)
{
    QRCode_genDataBytes_ALPHANUMERIC(raw,len,dataBytes,16);
};
static void QRCode_genEccCodePolyDiv(uint8_t* dataBytes,uint8_t* genBytes,uint8_t eccLen,uint8_t head)
{
    for (size_t i = 0; i < eccLen; i++)
    {
        uint16_t temp = (genBytes[i] + toGF256[head]);
        dataBytes[i] = dataBytes[i] ^ toUInt8[temp%255];
    }
};

static void QRCode_genGenCode(uint8_t* genCode,uint8_t len)
{
    for (int8_t i = 1; i < len; i++)
    {
        genCode[i] = (genCode[i-1]+i)%255;
        for (int8_t j = i-1; j > 0 ; j--)
        {
            genCode[j] = toGF256[toUInt8[(genCode[j-1]+i)%255]^toUInt8[genCode[j]]];
        }
        genCode[0] = toGF256[toUInt8[i]^toUInt8[genCode[0]]];
    }
    
};


static void QRCode_genEccCode(uint8_t* dataBytes,uint8_t dataLen,uint8_t eccLen)
{
    uint8_t *genCode=calloc(eccLen,1);
    QRCode_genGenCode(genCode,eccLen);
    
    // uint8_t genCode[10] = {251,67,46,61,118,70,64,94,32,45};
    // printf("genBytes: ");
    // for (size_t i = 0; i < eccLen; i++)
    // {
    //     printf("%d ",genCode[i]);
    // }printf("\n");
    // printf("dataBytes: ");
    // for (size_t i = 0; i < 16; i++)
    // {
    //     printf("%d ",dataBytes[i]);
    // }printf("\n");
    for (size_t i = 0; i < dataLen; i++)
    {
        if (dataBytes[i] == 0)
            continue;
        QRCode_genEccCodePolyDiv(dataBytes+i+1,genCode,10,dataBytes[i]);
    }
    // printf("eccBytes: ");
    // for (size_t i = 0; i < eccLen; i++)
    // {
    //     printf("%d ",dataBytes[dataLen+i]);
    // }printf("\n"); 
};

// TODO
static QRCode_Error QRCode_encode(QRCode *qr, const uint8_t *raw, uint16_t len, QRCode_Bitset *dataSet)
{
    printf("raw: %s\n",raw);

    uint8_t *dataBytes=calloc(26,1);
    QRCode_genDataBytes(qr,raw,len,dataBytes);

    dataSet->data = malloc(26);
    memcpy(dataSet->data,dataBytes,16);

    QRCode_genEccCode(dataBytes,16,10);

    memcpy(dataSet->data+16,dataBytes+16,10);
    
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


void QRCode_BitmapToUInt32Map(uint8_t *bitmap, uint8_t width, uint16_t len, uint32_t *intmap, uint32_t stride, uint32_t color)
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
