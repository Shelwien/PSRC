
uint f_dir; // chunk direction 0->probN, probN->0

Counter rf0[256];
enum{ u_max=12 };
Counter rf1[1];
Counter rff;

void rle_init( void ) {
  uint i;
  for( i=0; i<DIM(rf0); i++ ) rf0[i].P=M_rf0P0;
  for( i=0; i<DIM(rf1); i++ ) rf1[i].P=M_rf1P0;
  rff.P = M_rffP0;
  nsum.Init(); // used by freq_proc()
  f_dir = 0;
//  summin.P = Min(uint(mSCALE),M_smP0);
//  summax.P = Min(uint(mSCALE),M_sxP0);
}

template< class Rangecoder, class IOBase >
NOINLINE
int procbyte( Rangecoder& rc, IOBase* that, int c, int chunk ) {
  int b = 0;
  uint ctx;

  uint bit = 0;
  if( chunk<M_rf1th ) {
    int p1 = rff.P;
//    p1 = Max(1,Min(int(mSCALE),p1));
    bit = rc.Process( that, p1, c==0xFF );
    if( bit==1 ) {
      rff.update( 1, M_rffwr, M_rffmw ); c=0xFF;
    } else {
      rff.update( 0, M_rffwr, M_rffmw ); b=1;
    }
  }

  if( bit==0 ) {

    uint rFF = (chunk<M_rf1th) ? 0 : M_rf1ff1; 
    if( mode==0 ) if( c==0xFF ) c=rFF; else if( c>=rFF ) ++c;

    for( ; b<u_max; b++ ) {
      ctx = 0; //b==rFF; // rf1r(b);
      int p1 = rf1[ctx].P;
  //    p1 = Max(1,Min(int(mSCALE),p1));
      bit = rc.Process( that, p1, c==b );
      if( bit==1 ) {
        rf1[ctx].update( 1, M_rf1wr, M_rf1mw );
        break;
      }
      rf1[ctx].update( 0, M_rf1wr, M_rf1mw );
    }

    if( bit==1 ) c=b; else {
      uint z=0;
      for( b=7; b>=0; b-- ) {
        ctx = (0x100+z)>>(b+1);
        if( mode==0 ) bit = (c>>b)&1;
        if( z+(1<<b)-1<u_max ) bit=1; else {
          int p1 = rf0[ctx].P;
  //        p1 = Max(1,Min(int(mSCALE),p1));
          bit = rc.Process( that, p1, bit );
          rf0[ctx].Update( bit, M_rf0wr, M_rf0mw );   
        }
        z |= (bit<<b);
      } c = z;
    }

    if( mode==1 ) if( c==rFF ) c=0xFF; else if( c>rFF ) --c;
  }

  return c;
}

