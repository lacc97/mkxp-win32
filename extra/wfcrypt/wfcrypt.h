#ifndef WFCRYPT_H
#define WFCRYPT_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

WIN32_API int wfcrypt_encrypt(char* plainText, int nb, int nk, const char* iv, const char* key, long plainTextLength);
WIN32_API int wfcrypt_decrypt(char* cipherText, int nb, int nk, const char* iv, const char* key, long cipherTextLength);

#ifdef __cplusplus
}
#endif

#endif
