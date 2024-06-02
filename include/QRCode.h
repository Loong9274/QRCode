#ifndef __QRCODE_H
#define __QRCODE_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
#include <stdint.h>

    typedef enum
    {
        QRCODE_OK,
        QRCODE_ERROR_VERSION_NOT_MATCH,
        QRCODE_ERROR_MODE_NOT_MATCH,
        QRCODE_ERROR_NO_MEMORY,
    } QRCode_Error;

    typedef enum
    {
        QRCODE_MODE_NOSPECIFY,
        QRCODE_MODE_NUMERIC,
        QRCODE_MODE_ALPHANUMERIC,
        QRCODE_MODE_BYTE,
        QRCODE_MODE_KANJI,
        QRCODE_MODE_ECI
    } QRCode_Mode;

    typedef enum
    {
        QRCODE_ECC_NOSPECIFY,
        QRCODE_ECC_L,
        QRCODE_ECC_M,
        QRCODE_ECC_Q,
        QRCODE_ECC_H
    } QRCode_Ecc;

    typedef enum
    {
        QRCODE_MASK_NOSPECIFY,
        QRCODE_MASK_TYPE1,
        QRCODE_MASK_TYPE2,
        QRCODE_MASK_TYPE3,
        QRCODE_MASK_TYPE4,
        QRCODE_MASK_TYPE5,
        QRCODE_MASK_TYPE6,
        QRCODE_MASK_TYPE7,
        QRCODE_MASK_TYPE8
    } QRCode_Mask;

    typedef struct
    {
        QRCode_Mode mode;
        QRCode_Ecc ecc;
        QRCode_Mask mask;
        uint8_t *data;
        uint8_t version;
    } QRCode;

    QRCode *QRCode_generate(const uint8_t *raw,
                            uint16_t len,
                            QRCode_Mode mode,
                            QRCode_Ecc ecc,
                            uint8_t version,
                            QRCode_Mask mask);

    void testQRCode(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__QRCODE_H