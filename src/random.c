#include "random.h"

static unsigned long int randrsl[256] = {0}, randcnt;
static unsigned long int mm[256] = {0};
static unsigned long int aa = 0, bb = 0, cc = 0;

void isaac(unsigned long int* buffer, int length)
{
  register unsigned long int i,x,y;

  cc = cc + 1;
  bb = bb + cc;

  for (i = 0; i < 256; ++i){
    x = mm[i];
    switch (i % 4){
      case 0: aa = aa^(aa<<13); break;
      case 1: aa = aa^(aa>>6); break;
      case 2: aa = aa^(aa<<2); break;
      case 3: aa = aa^(aa>>16); break;
    }
    aa = mm[(i+128)%256] + aa;
    mm[i] = y = mm[(x>>2)%256] + aa + bb;
    randrsl[i] = bb = mm[(y>>10)%256] + x;
  }

  for(i = 0; i < length && i < 256; i++){
    buffer[i] = randrsl[i];
  }
}

#define mix(a,b,c,d,e,f,g,h) \
{ \
  a^=b<<11; d+=a; b+=c; \
  b^=c>>2;  e+=b; c+=d; \
  c^=d<<8;  f+=c; d+=e; \
  d^=e>>16; g+=d; e+=f; \
  e^=f<<10; h+=e; f+=g; \
  f^=g>>4;  a+=f; g+=h; \
  g^=h<<8;  b+=g; h+=a; \
  h^=a>>9;  c+=h; a+=b; \
}

void rand_init()
{
  int i;
  unsigned long int a,b,c,d,e,f,g,h;
  aa=bb=cc=0;
  a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

  for(i=0; i<4; ++i){
   mix(a,b,c,d,e,f,g,h);
  }

  for(i=0; i<256; i+=8){
    a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
    e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
    mix(a,b,c,d,e,f,g,h);
    mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
    mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
  }

  for(i=0; i<256; i+=8){
    a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
    e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
    mix(a,b,c,d,e,f,g,h);
    mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
    mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
  }

  isaac(0, 0);
  randcnt = 256; 
}


void set_key(char* key, int length){
  for(int i = 0; i < 1024 && i < length; i += 4){
    mm[i / 4] = (key[i] << 24) + (key[i + 1] << 16) + (key[i + 2] << 8) + key[i + 3];
  }
}