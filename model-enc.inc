
NOINLINE
void model_enc( void ) {

  enum { shift=5, mask=(1<<shift)-1, sSCALE=1<<shift };

  LinearMixer mix( M_mW0 );

  uint pp=0, p=0;

  for( uint k=0; k<f_len; ) {
    uint l = k>0 ? Min(k+max_quants/8,f_len)-k : Min(fast_quants/8,f_len); // block length
    k += l;

    for( uint i=0; i<l; i++ ) {

      const uint c = get();

      uint ctx=1; int b=(c<<24);
      do {

        uint p1 = mix.Mixup(f0[ctx].P,f1[p][ctx].P);
        p1 = qmas[(p1+mask)>>shift];
        
        if( b<0 ) { 
          put0( p1+0x80 );
          f0[ctx].update( 1, M_f0wr, M_f0mw );   
          f1[p][ctx].update( 1, M_f1wr, M_f1mw ); 
          ctx += ctx + 1;
        } else {
          put0( p1 );
          f0[ctx].update( 0, M_f0wr, M_f0mw );   
          f1[p][ctx].update( 0, M_f1wr, M_f1mw ); 
          ctx += ctx + 0;
        }

        b <<= 1;
      } while( ctx<256 );

      pp = p; p = c;
    }

    yield(this,2);
  }
}
