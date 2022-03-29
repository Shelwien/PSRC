
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <windows.h>

#pragma pack(1)

typedef unsigned int   uint;
typedef unsigned short word;
typedef unsigned char  byte;
typedef unsigned long long int qword;

#ifndef __INTEL_COMPILER
#define restrict __restrict
#endif

#ifdef __GNUC__
 #define NOINLINE __attribute__((noinline))
 #define ALIGN(n) __attribute__((aligned(n)))
#else
 #define NOINLINE __declspec(noinline)
 #define ALIGN(n) __declspec(align(n))
#endif

template <typename T1, typename T2> T1 Min( T1 t1, T2 t2 ) { return t1<t2?t1:t2; }
template <typename T1, typename T2> T1 Max( T1 t1, T2 t2 ) { return t1>t2?t1:t2; }
template <class T,int N> int DIM( T (&wr)[N] ) { return sizeof(wr)/sizeof(wr[0]); };

#define AlignUp(x,r) ((x)+((r)-1))/(r)*(r)


uint flen( FILE* f ) {
  fseek( f, 0, SEEK_END );
  uint len = ftell(f);
  fseek( f, 0, SEEK_SET );
  return len;
}

uint log2i( uint x ) {
#ifdef __GNUC__
  uint i; 
  for( i=0; i<32; i++,x>>=1 ) if( x==0 ) break;
  return i-1;
#else
  return _bit_scan_reverse(x);
#endif
}

template <class T>
  T* mAlloc( int Size ) 
    { return static_cast<T*>(VirtualAlloc(0,sizeof(T)*Size,0x1000,0x04)); }
