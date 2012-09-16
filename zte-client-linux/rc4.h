#ifndef _RC4_H_
#define _RC4_H_

struct rc4_state
{
    int x, y, m[256];
};

void rc4_setup( struct rc4_state *s, unsigned char *key,  int length );

void rc4_crypt( struct rc4_state *s, unsigned char *data, int length );

#endif
