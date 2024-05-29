#include "QRCode.h"
#include <stdlib.h>

typedef struct
{
    uint8_t *data;
    uint16_t index;
    uint16_t len;
}QRCode_Bitset;

// TODO
static QRCode_Error QRCode_analyse(QRCode* qr,const uint8_t *raw,uint16_t len)
{
    qr->version = 1;
    qr->ecc = QRCODE_ECC_M;
    qr->mode = QRCODE_MODE_ALPHANUMERIC;
    return QRCODE_OK;
};

// TODO
static QRCode_Error QRCode_encode(QRCode* qr,const uint8_t *raw,uint16_t len,QRCode_Bitset *dataSet)
{

    return QRCODE_OK;
};

// TODO
static QRCode_Error QRCode_place(QRCode* qr,QRCode_Bitset* dataSet)
{
    uint16_t bufferSize = ((qr->version*4+17)*(qr->version*4+17)+7)/8;
    qr->data = calloc(bufferSize,sizeof(uint8_t));
    for (size_t i = 0; i < bufferSize; i++)
    {
        qr->data[i] = 0xAA;
    }
    
    return QRCODE_OK;
};

QRCode *QRCode_generate(const uint8_t *raw,
                      uint16_t len,
                      QRCode_Mode mode,
                      QRCode_Ecc ecc,
                      uint8_t version,
                      QRCode_Mask mask)
{
    QRCode_Bitset *dataSet;
    QRCode* qr = (QRCode*)calloc(1,sizeof(QRCode));
    qr->mode = mode;
    qr->ecc = ecc;
    qr->version = version;
    qr->mask = mask;
    // check mode, version
    if (QRCode_analyse(qr,raw,len) != QRCODE_OK)
    {
        return NULL;
    }
    if (QRCode_encode(qr,raw,len,dataSet) != QRCODE_OK)
    {
        return NULL;
    }
    if (QRCode_place(qr,dataSet) != QRCODE_OK)
    {
        return NULL;
    }else{
        return qr;
    }

};






#include <stdio.h>
void testQRCode(void)
{
    printf("Hello QRCode\n");
    return;
}
