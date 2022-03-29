
template< class Rangecoder, class IOBase >
NOINLINE
uint rle_put( Rangecoder& rc, IOBase* that, byte* p, uint l ) {
  uint c,i,I,j,k,x;
  uint count[probN];  // RLE byte counts

  bitsort( p, (uint*)&p[quantbufsize], l, count );

  freq_proc(rc,that, &count[0],probN, max_quants );

  uint offs[probN+1];
  for( i=0,x=0; i<probN; i++ ) offs[i]=x,x+=count[i]; offs[i]=x;
  for( j=0; j<probN; j++ ) {
    uint k = f_dir ? probN-1-j : j;
    for( i=offs[k]; i<offs[k+1]; i++ ) procbyte(rc,that,p[i],k);
  }

  f_dir ^= 1;

  return x;
}
