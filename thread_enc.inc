	
struct thread_enc : thread<thread_enc> {

  rle_class0* pR;
  filehandle g;
  volatile uint f_EOF;

  void thread( void ) {
    SetThreadAffinityMask( GetCurrentThread(), 8 );

    rle_class0& R = *pR;
    R.Init();
    R.addout( fout.out(), outbufsize );

    while(1) {

      byte* p;
      while( (p=tmp.inp())==0 ) {
        if( f_EOF!=0 ) { R.f_EOF=1; break; }
        wait();
      }

      if( p ) R.addinp( p, tmp.getsize(p) );

      int Rr;
      while(1) {
        Rr = R.coro_call(&R); // process
        if( Rr!=2 ) break; // normally waits for more input
        fout.setsize( (byte*)R.outbeg, outbufsize ); fout.push(); // allow to write the prev buffer
        while( (p=fout.out())==0 ) wait();
        R.addout( p, outbufsize ); // next buffer
      }

      if( Rr==0 ) break;

      tmp.pop(); // free the buffer
    }

    fout.setsize( (byte*)R.outbeg, (R.outptr-R.outbeg)|(1<<31) ); fout.push();
  }

};

thread_enc th_enc;

void processfile0( filehandle f, filehandle g, uint _f_len ) {

  tmp.init();
  SetThreadAffinityMask( GetCurrentThread(), 1 );

  finp.init(); finp.a--;
  th_inp.f = f;
  th_inp.start();

  fout.init();
  th_out.g = g;
  th_out.start();

  rle_class0* R0 = mAlloc<rle_class0>(1); rle_class0& R = *R0;

  th_enc.pR = &R;
  th_enc.g = g;
  th_enc.start();

  Model0* M0 = mAlloc<Model0>(1); Model0& M = *M0;
  M.f_len = _f_len;
  M.coro_init();
  M.addout( tmp.out(), max_quants );

  while( 1 ) {
    int Mr = M.coro_call(&M);
    if( Mr==1 ) {

      finp.pop();

      byte* p;
      while( (p=finp.inp())==0 ) th_inp.wait();
      uint l = finp.getsize(p);
      if( l==0 ) break; // get/put don't support repeated failures
      M.addinp( p, l ); 

    } else { // 0 or 2

      tmp.setsize( (byte*)M.outbeg, M.getoutsize() ); tmp.push(); 

      if( Mr==0 ) th_enc.f_EOF=1; // send EOF

      byte* p;
      while( (p=tmp.out())==0 ) th_enc.wait();
      M.addout( p, max_quants );
      if( Mr==0 ) break;
    }
  }
  th_enc.quit();
  th_out.quit();
}

