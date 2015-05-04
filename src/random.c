#include "random.h"

typedef  unsigned long int  ub4;

static ub4 randrsl[256], randcnt;
static ub4 mm[256];
static ub4 aa=0, bb=0, cc=0;

void isaac(unsigned long int buffer[], int length)
{
  register ub4 i,x,y;

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
  ub4 a,b,c,d,e,f,g,h;
  a = b = c = d = e = f = g = h = 0x9e3779b9;

  for (i = 0; i < 4; ++i){
    mix(a,b,c,d,e,f,g,h);
  }

  for (i = 0; i < 256; i += 8){
    mix(a,b,c,d,e,f,g,h);
    mm[i] = a; 
    mm[i + 1] = b; 
    mm[i + 2] = c; 
    mm[i + 3] = d;
    mm[i + 4] = e; 
    mm[i + 5] = f; 
    mm[i + 6] = g; 
    mm[i + 7] = h;
  }

  isaac(0, 0);
  randcnt = 256;
}


void set_key(unsigned long int key[], int length){
  switch(length){
    case 1:
      aa = key[0];
      break;
    case 2:
      aa = key[0];
      bb = key[1];
      break;
    case 3:
      aa = key[0];
      bb = key[1];
      cc = key[2];
      break;
    default:
      aa = bb = cc = 0;
  }
}