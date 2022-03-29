
#ifndef rle_state_DEFINED
#define rle_state_DEFINED
struct rle_state {
  byte* restrict ptr[probN];  // RLE chunk pointers for decoding
};
#endif

struct rle_class : coroutine<2> {

  volatile uint f_EOF;
//  Counter  eof;

  #include "model_sort.inc"
  #include "model_freq.inc"
  #include "model_rle1.inc"
  #include "model_rle_get.inc"
  #include "model_rle_put.inc"

  void do_process( void ) {
    f_EOF = 0;
    rle_init();
    ALIGN(16) 
    Rangecoder<mode> rc;
    if( mode==1 ) yield(this,1); // Init would yield here in decoding
    rc.rc_Init(this); // get() would yield 1 on decode 
    while( f_EOF==0 ) {
      if( mode==1 ) { 
        uint bit = rc.Process( this, mSCALE, 0 );
        if( bit ) f_EOF=1; else {
          rle_get( rc,this, (byte*)outbeg ); 
          yield(this,2); 
        }
      }
      if( mode==0 ) { 
        yield(this,1); if( f_EOF ) break;
        uint bitcount = getinpleft();
        if( bitcount>0 ) {
          rc.Process( this, mSCALE, 0 );
          rle_put( rc,this, (byte*)inpbeg,bitcount ); 
        }
      }
    }
    if( mode==0 ) rc.Process( this, mSCALE, 1 ); // encode EOF
    rc.rc_Quit(this);
    yield(this,0);
  }

  void Init( void ) {
    coro_init();
    coro_call(this); 
  }

};
