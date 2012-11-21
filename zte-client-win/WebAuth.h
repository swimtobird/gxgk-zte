#ifndef WEBAUTH_H
#define WEBAUTH_H

int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen);

char* WebAuth(const char * username, const char *password, const char *ip, const char *url);
char * WebLogout(const char *ip, const char *base_url);
#endif