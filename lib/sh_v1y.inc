
enum {
  SCALElog = 15,
  SCALE    = 1<<SCALElog,
  eSCALE = 16*SCALE,
  hSCALE = SCALE/2,
  mSCALE = SCALE-1
};

template< int mode >
struct Rangecoder {

  enum {
    NUM   = 4,
    sTOP  = 0x01000000U,
    gTOP  = 0x00010000U,
    Thres = 0xFF000000U,
    Threg = 0x00FF0000U
  };

  uint  range;
  uint  rprec;
  qword lowc;
  uint  code; 
  uint  FFNum;
  uint  Cache;

  template< class IOBase >
  uint Process( IOBase* that, uint freq, uint b ) { 

//    freq = sq[ 512+st[freq] ];
//    freq = sq[ qmap[512+st[freq]] ];
//    freq = qmap[freq];

    uint rnew = rprec*freq;
//    uint rnew = (qword(range)*(freq<<(32-SCALElog)))>>32;

    if( mode ) b = (code>=rnew);

    range = ((range-rnew-rnew)&(-b)) + rnew;
    rnew &= -b;

    if( mode ) code -= rnew; else lowc += rnew;

    if( mode ) {
      while( range<sTOP ) range<<=8, (code<<=8)+=that->get();
    } else {
      while( range<sTOP ) { range<<=8; ShiftLow(that); }
    }

    rprec = range>>SCALElog;

    return b;
  }

  template< class IOBase >
  void ShiftLow( IOBase* that ) {
    uint Carry = uint(lowc>>32);
    uint low = uint(lowc);
    if( low<Thres || Carry ) {
      that->put( Cache+Carry );
      for(; FFNum!=0; FFNum-- ) that->put( Carry-1 );
      Cache = low>>24;
      Carry = 0;
    } else FFNum++;
    lowc = (low<<8);
  }

  template< class IOBase >
  void rc_Init( IOBase* that ) {
    range = 0xFFFFFFFF;
    rprec = range>>SCALElog;
    lowc  = 0;
    FFNum = 0;
    Cache = 0;
    if( mode==1 ) {
      for(int _=0; _<NUM+1; _++) (code<<=8)+=that->get(); 
    }
  }

  template< class IOBase >
  void rc_Quit( IOBase* that ) {
    if( mode==0 ) {
      for(int _=0; _<NUM+1; _++) ShiftLow(that); 
    }
  }

};

