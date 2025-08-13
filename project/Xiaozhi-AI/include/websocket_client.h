#ifndef _WEBSOCKET_CLIENT_H_
#define _WEBSOCKET_CLIENT_H_

#include "common.h"

#define WEBSOCKET_SUBPROTOCOL_MAX 2

typedef struct _cwebsocket {
	int fd;
	int retry;
	char *uri;
	uint8_t flags;
	uint8_t state;
	char *proxy_addr;
	char *proxy_port;
	char **headers;
	int num_headers;
	mbedtls_ssl_context ssl;
	mbedtls_net_context server_fd;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cacert;
	mbedtls_x509_crt clicert;
	mbedtls_pk_context pkey;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_session *session;
	int code;
	size_t subprotocol_len;
	cwebsocket_subprotocol *subprotocol;
	cwebsocket_subprotocol *subprotocols[WEBSOCKET_SUBPROTOCOL_MAX];
} cwebsocket_client;

typedef struct {
	cwebsocket_client *socket;
	cwebsocket_message *message;
} cwebsocket_client_thread_args;

int websocket_test(void);
int cwebsocket_client_init(cwebsocket_client *websocket, cwebsocket_subprotocol **subprotocols, int subprotocol_len) ;
int cwebsocket_client_read_data(cwebsocket_client *websocket);
void cwebsocket_client_close(cwebsocket_client *websocket) ;
int cwebsocket_client_write_data(cwebsocket_client *websocket, const char *message,uint64_t message_len,opcode code );

#endif // _WEBSOCKET_CLIENT_H_