
void bitsort( byte* buf, uint* tmp, uint l, uint (&count)[probN] ) {
  uint i,x;

  uint bofs[probN+1];
  uint freq[256+8]; // prob freqs

  // compute offsets of sorted chunks
  for( i=0; i<DIM(freq); i++ ) freq[i]=0;
  for( i=0; i<l; i++ ) freq[1+buf[i]]++;
  for( i=1; i<=probN; i++ ) freq[i]+=freq[0x80+i];

  for( i=1; i<=probN; i++ ) freq[i]=(freq[i]+31)/32; // align 32 bits
  for( i=0; i<probN; i++ ) freq[i+1]+=freq[i]; // turn into offsets
  for( i=0; i<=probN; i++ ) bofs[i]=freq[i]; // bit chunk ofs copy

  {
    uint code[probN];
    for( i=0; i<probN; i++ ) code[i]=1;
    for( i=0; i<l; i++ ) { // sort
      uint c = buf[i];
      uint p = c&0x7F, bit = c>>7;
      uint r = code[p];
      uint q = r+r+bit;
      if( (q>>1)<r ) tmp[freq[p]++]=q, q=1;
      code[p] = q;
    }
    for( i=0; i<probN; i++ ) { // flush
      uint c = code[i];
      if( c>1 ) tmp[freq[i]] = ~((~c)<<(32-log2i(c))); // no redundant flush
    }
  }

  // inplace rle
  for( i=0,x=0; i<probN; i++ ) {
    uint rl = 0, y=x; // run length
    uint j=bofs[i], z=bofs[i+1];
    for( j=j; j<z; j++ ) {
      uint k, c = tmp[bofs[i]++];
      for( k=0; k<32; k++,c+=c ) {
        if( int(c)<0 ) rl++; else {
          while( rl>=255 ) buf[x++]=0xFF, rl-=255;
          buf[x++]=rl, rl=0;
        }
      } 
    }
    while( rl>255*max_trail_FFs ) buf[x++]=0xFF, rl-=255;
    count[i]=x-y; // c runs for j's prob
  }

}

