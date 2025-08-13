#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__
#include<stdint.h>

extern char challenge[50];
extern char serial_number[64];
extern char license_key[64];

int https_check_active(char *clientID,char *devID,char *acktiveServer,char *acktivePort,char *acktiveUrl );
int https_check_active_enterprise(char *clientID,char *devID,char *acktiveServer,char *acktivePort,char *acktiveUrl );

#endif // !__HTTP_REQUEST_H__
