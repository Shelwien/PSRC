
template< class Rangecoder, class IOBase >
NOINLINE
void rle_get( Rangecoder& rc, IOBase* that, byte* p ) {
  uint i,j,x,y;
  uint count[probN];  // RLE byte counts

  rle_state& rs = (rle_state&)p[0];

  // fetch quant pool size table
  freq_proc(rc,that, &count[0],probN, max_quants );

  byte* restrict q = &p[sizeof(rle_state)];
  for( j=0; j<probN; j++ ) {
    uint k = f_dir ? probN-1-j : j;
    rs.ptr[k] = q;
    uint y = count[k];
    uint bit0 = (k&1), bit1 = bit0^1;
    for( i=0; i<y; i++ ) {
      uint c = procbyte(rc,that,0,k);
      byte* r = q+c;
      while( q<r ) *q++=bit1;
      if( c<255 ) *q++=bit0;
    }
    // add max_trail_FFs after each segment
    for( i=0; i<255*(max_trail_FFs+1); i++ ) *q++=bit1;
  }

  f_dir ^= 1;
}

	