
struct Model : coroutine<1> {

  enum{ CNUM=256 };
  ALIGN(32)
  Counter f0[CNUM];       // order0 freqs
  ALIGN(32)
  Counter f1[CNUM][CNUM]; // order1 freqs

  uint f_len;

  #include "model-dec.inc"
  #include "model-enc.inc"

  void do_process( void ) {

    init_qmar();

    uint i,j;
    for( i=0; i<CNUM; i++ ) {
      f0[i].P = Max(1,Min(int(mSCALE),M_f0P0));
      for( j=0; j<CNUM; j++ ) f1[i][j].P=Max(1,Min(int(mSCALE),M_f1P0));
    }

    mode ? model_dec() : model_enc();

    yield(this,0);
  }
                            	
};

