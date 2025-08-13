#ifndef __HMAC_SHA256_H__
#define __HMAC_SHA256_H__

// SHA-256 常量和函数定义
#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

void hmac_sha256_string(const char *key, const char *data, char *result);

#endif