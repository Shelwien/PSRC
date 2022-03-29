
#include <string.h>

struct mapping {
  int  left;
  int  Size,MSize;
  int* pmap;
  int  value;

  mapping( int L, const char* S1 ) {
    const char* S=S1;
    S += strlen(S)+1;
    left = L-1;
    MSize = strlen(S);
    pmap = new int[MSize+1];
    int i,j;
    for( i=0,j=0,value=0; i<MSize; i++ ) {
      pmap[i]=j, j+=(S[i]&1);
      (value<<=1)+= (S[i]&1);
    }
    pmap[i]=j;
//    value>>=1;
    Size=j+1;
    value += L;
  }

  int map( int x ) {
    x-=left;
    if( x<0 ) x=0;
    if( x>MSize ) x=MSize;
    return pmap[x];
  }

  //y = y*M_FF_y_r.MSize + M_FF_y_r.map(r);
  template< class INT >
  void inc( INT& y, int r ) {
    y = y*Size + map(r);
  }

};

struct masking {
  int  Size,SMSize,bits;
  int* pmap;

  masking( const char* S1 ) {
    const char* S=S1;
    S += strlen(S)+1;
    int MSize = strlen(S);
    uint mask;
    int i,j;
    for( i=0,mask=0,bits=0; i<MSize; i++ ) {
      (mask<<=1) += (S[i]&1); 
      bits += (S[i]&1);
    }
    SMSize = 1<<MSize;
    pmap = new int[SMSize];
    Size = 1<<bits;
    for( i=0,j=0; i<Size; i++ ) pmap[j]=i, j = ((j|(~mask))+1)&mask;
    for( i=0; i<SMSize; i++ ) pmap[i]=pmap[i&mask];
  }

  int map( int x ) {
    if( x<0 ) x=0;
    if( x>=SMSize ) x=SMSize-1;
    return pmap[x];
  }

  void inc( int& y, int r ) {
    (y<<=bits) += pmap[r];
  }

};

struct masking_b {
  int  Size,SMSize;
  int* pmap;

  masking_b( const char* S1 ) {
    const char* S=S1;
    S += strlen(S)+1;
    int MSize = strlen(S);
    uint mask;
    int b,i,j,k;
    for( i=0,mask=0; i<MSize; i++ ) {
      (mask<<=1) += (S[i]&1); 
//      bits += (S[i]&1);
    }
    SMSize = 1<<MSize;
    pmap = new int[SMSize];
    for( i=0; i<SMSize; i++ ) pmap[i]=0;
    for( b=MSize-1,k=0; b>=0; b-- ) {
      for( i=(1<<(MSize-1-b)); i<(2<<(MSize-1-b)); i++ ) {
        j = (i&(int(~mask)>>(b+1)));
        pmap[i] = ( j<i ) ? pmap[j] : k++;
      }
    }
    Size = k;
  }

  int map( int x ) {
    if( x<1 ) x=1;
    if( x>=SMSize ) x=SMSize-1;
    return pmap[x];
  }

  void inc( int& y, int r ) {
    (y*=Size) += map(r);
  }

};

// profiling version
#define pdesc( Desc, Ofs, Map ) mapping Desc( Ofs, "!MAP!"#Desc"!"#Ofs"\x00"Map )

#define pmask( Desc, Map ) masking Desc( "!MAP!"#Desc"!0\x00"Map )

#define pmask2( Desc, Map ) masking_b Desc( "!MAP!"#Desc"!0\x00"Map )


