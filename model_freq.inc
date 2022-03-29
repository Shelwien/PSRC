
template <int len, int _rlen>
struct NumberBits {
  enum{ rlen=_rlen };
  Counter P[1<<rlen];
  void Init( void ) {
    uint i;
    for( i=0; i<DIM(P); i++ ) P[i].P=hSCALE;
  }
};

template< class NumBits, class Rangecoder, class IOBase >
NOINLINE
uint numbits_proc( Rangecoder& rc, IOBase* that, NumBits& X, uint n, uint clen, int lo, int hi, const int wr, const int mw ) {
  int i,j,k,z=0;
  const uint lim = 1<<X.rlen;
  for( i=clen-1; i>=0; i-- ) {
    j = ((1<<clen)+z)>>(i+1);
    k = (n>>i)&1;
    if( z+(1<<i)-1<=lo ) k=1; else
    if( z+(1<<i)  >=hi ) k=0; else {
      k = rc.Process( that, (j<lim?X.P[j].P:hSCALE), k );
      if( j<lim ) X.P[j].Update( k, wr, mw );
    }
    z |= (k<<i);
  }
  return z; // n = z;
}

template< class NumBits, class Rangecoder, class IOBase >
NOINLINE
uint xnumbits_proc( Rangecoder& rc, IOBase* that, NumBits& X, uint n, int lo, int hi, const int wr, const int mw ) {
  uint l = log2i(hi-1);
  uint a = numbits_proc(rc,that,X,n, l+1,lo,hi,wr,mw);
  return a;
}


template< class Rangecoder, class IOBase >
NOINLINE
uint number_proc( Rangecoder& rc, IOBase* that, uint x, uint max ) {
  uint i;
  for( i=0; i<sizeof(x); i++ ) ((byte*)&x)[i]=procbyte(rc,that,((byte*)&x)[i]);
  return x;
}

NumberBits<max_quants_log,5> nsum;
Counter summin, summax;

template< class Rangecoder, class IOBase >
NOINLINE
uint freq_proc( Rangecoder& rc, IOBase* that, uint* freq, int N, uint max, uint sum=0, uint level=0 ) { // uint freq[0..N-1] to encode
  uint i;

  if( level==0 ) {
    summin.P = Min(uint(mSCALE),M_smP0);
    summax.P = Min(uint(mSCALE),M_sxP0);
  }

  if( max>=sum ) {
    if( mode==0 ) for( i=0,sum=0; i<N; i++ ) sum+=freq[i];
    uint flag = 0;
    flag = rc.Process( that, summax.P, sum==max );
    summax.Update( flag, M_sxwr, M_sxmw );
    if( flag ) sum=max; else {
//      sum = xnumbits_proc( rc,that,nsum,sum,-1,max, M_n0wr, M_n0mw );
      flag = rc.Process( that, summin.P, sum==0 );
      summin.Update( flag, M_smwr, M_smmw );
      if( flag ) sum=0; else {
        sum = xnumbits_proc( rc,that,nsum,sum, 0,max, M_n0wr, M_n0mw );
      }
    }
  }

  if( N<=1 ) freq[0]=sum; else
  if( sum==0 ) {
    if( mode==1 ) for( i=0; i<N; i++ ) freq[i]=0;
  } else {
    uint h = N>>1;
    uint sum0 = freq_proc( rc,that, &freq[0],h,  sum,0, level+1 );
    freq_proc( rc,that, &freq[h],N-h, 0,sum-sum0, level+1 );
  }

  return sum;
}



