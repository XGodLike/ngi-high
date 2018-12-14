#ifndef _CRYPT_HEADER_FILE_
#define _CRYPT_HEADER_FILE_

#ifdef __cplusplus
extern "C" {
#endif

int vcyber_encrypt(const char * key, const void * src, int src_len, char * dst, int dst_len);
int vcyber_decrypt(const char * key, const char * src, int src_len, void * dst, int dst_len);

#ifdef __cplusplus
}
#endif

#endif
