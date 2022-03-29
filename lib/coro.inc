
//#include <setjmp.h>
//#include "my_setjmp.h"
//#include "my_setjmp2.h"
#include "my_setjmp2a.h"

void yield( void* p, int value );

template< int STKPAD_C=1 >
struct coroutine {

  enum{ STKPAD=STKPAD_C<<16 };

  volatile byte* outptr;
  volatile byte* outend;
  volatile byte* outbeg;

  volatile uint  state;

  volatile byte* inpptr;
  volatile byte* inpend;
  volatile byte* inpbeg;

  ALIGN(32) jmp_buf PointA;
  ALIGN(32) jmp_buf PointB;

  template <typename T> 
  NOINLINE // necessary to avoid allocation of do_process vars before stktmp
  void call_do_process( T* that ) {
    that->do_process();
  }

  template <typename T> 
  NOINLINE // necessary for MSC, to avoid allocation of stktmp before setjmp()
  void call_do_process0( T* that ) {
    byte stktmp[STKPAD]; 
    state = stktmp-((byte*)0);
    call_do_process(that);
  }

  template <typename T> 
  uint coro_call( T* that ) {
    if( setjmp(PointA)==0 ) {
      if( state ) longjmp(PointB,1); 
      call_do_process0(that);
    }
    return state;
  }

  void chkinp( void ) { if( inpptr>=inpend ) yield(this,1); }

  void chkout( void ) { if( outptr>=outend ) yield(this,2); }
/*
  template <int f_limit>
    byte get( void ) { if( f_limit ) chkinp(); return *inpptr++; }

  template <int f_limit>
    void put( uint c ) { *outptr++ = c; if( f_limit ) chkout(); }
*/
  byte get( void ) { chkinp(); return *inpptr++; }

  void put( uint c ) { *outptr++ = c; chkout(); }
  void put0( uint c ) { *outptr++ = c; }

  void coro_init( void ) {
    inpptr=inpbeg=inpend = 0;
    outptr=outbeg=outend = 0;
    state = 0;
  }

  uint getinpleft() { return inpend-inpptr; }
  uint getinpsize() { return inpptr-inpbeg; }
  uint getoutleft() { return outend-outptr; }
  uint getoutsize() { return outptr-outbeg; }

  void addinp( byte* inp,uint inplen ) {
    inpbeg = inpptr = inp;
    inpend = &inp[inplen];
  }

  void addout( byte* out,uint outlen ) {
    outbeg = outptr = out;
    outend = &out[outlen];
  }

};

template< class coroutine >
NOINLINE
void yield( coroutine* p, int value ) { 
//  coroutine& q = *(coroutine*)p;
  coroutine& q = *p;
  if( setjmp(q.PointB)==0 ) { q.state=value; longjmp(q.PointA,1); }
}
