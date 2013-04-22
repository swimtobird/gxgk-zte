#ifndef WEBAUTH_H
#define WEBAUTH_H

int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen);

char* WebAuth(const char * username, const char *password, const char *ip, const char *url, const int timeout);
char * WebLogout(const char *ip, const char *base_url, const int timeout);
#endif