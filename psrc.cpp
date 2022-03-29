
#include "common.inc"

#include "file_api.inc"

#include "sh_v1y.inc"

#include "sh_mapping.inc"
#include "MOD/sh_model_h.inc"
#include "MOD/sh_model_p.inc"

#include "qmaq1.inc"

#include "coro.inc"

//#define SET_AFF
#define SetThreadAffinityMask(x,y)

enum {
  probN = 128, max_trail_FFs=2, //513,
  max_quants_log = 23,
  max_quants = (1<<max_quants_log), // <2^N to be able to store the value to *_log bits
  quantbufsize = (1<<max_quants_log),
  fast_quants = max_quants,
  rlebufN = 4, inpbufN = 16, outbufN = 16
};

#include "bufring.inc"

#include "counter.inc"

#define mode 0
#define Model Model0
#define rle_class rle_class0
#include "model_rle.inc"
#include "model.inc"
#undef rle_class
#undef Model
#undef mode

#define mode 1
#define Model Model1
#define rle_class rle_class1
#include "model_rle.inc"
#include "model.inc"
#undef rle_class
#undef Model
#undef mode

enum { 
  tmpbufsize_dec = AlignUp( quantbufsize + 2*255*max_trail_FFs*probN + sizeof(rle_state) +4, 4096 ),
  tmpbufsize_enc = AlignUp( quantbufsize + quantbufsize/8 + 4*probN +4, 4096 ),
  tmpbufsize = (tmpbufsize_dec<tmpbufsize_enc) ? tmpbufsize_enc : tmpbufsize_dec // max
};
typedef t_bufring<rlebufN,tmpbufsize> bufring;

enum {
  inpbufsize = 1<<16, inpbufsize1 = AlignUp(inpbufsize+4,4096),
  outbufsize = 1<<16, outbufsize1 = AlignUp(outbufsize+4,4096)
};

typedef t_bufring<inpbufN,inpbufsize1> inpring;
typedef t_bufring<outbufN,outbufsize1> outring;

//ALIGN(4096) bufring tmp;
//ALIGN(4096) inpring finp;
//ALIGN(4096) outring fout;

bufring& tmp  = * mAlloc<bufring>(1);
inpring& finp = * mAlloc<inpring>(1);
outring& fout = * mAlloc<outring>(1);

#include "thread.inc"
#include "thread_inp.inc"
#include "thread_out.inc"
#include "thread_dec.inc"
#include "thread_enc.inc"

int main( int argc, char** argv ) {

  if( argc<4 ) return 1;
  filehandle f( argv[2],0 ); if( f==0 ) return 1;
  filehandle g( argv[3],1 ); if( g==0 ) return 2;
  uint f_len = 0;

  if( argv[1][0]=='c' ) {
    f_len = f.size();
    g.writ( f_len );
    processfile0( f, g, f_len );
  } else {
    f.read( f_len );
    processfile1( f, g, f_len );
  }

  f.close();

/*
{
  int i,s;
  byte flag[probN]; memset(flag,1,sizeof(flag));
  for( i=0,s=0; i<sizeof(qmaq); i++ ) s+=flag[qmaq[i]], flag[qmaq[i]]=0;
  g.writ( &tmp, s*10 );
//  for( i=0; i<sizeof(flag); i++ ) printf( "%i", flag[i] );
}
*/

  g.close();

  return 0;
}

