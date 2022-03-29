
struct thread_dec : thread<thread_dec> {

  rle_class1* pR;
  filehandle f;

  void thread( void ) {
    SetThreadAffinityMask( GetCurrentThread(), 8 );
    rle_class1& R = *pR;
    R.Init();

    while( R.f_EOF==0 ) {

      byte* p;
      while( (p=tmp.out())==0 ) {
        if( R.f_EOF!=0 ) break;
        wait();
      }

      if( p ) R.addout( p, 0 ); // size is not used

      int Rr;
      while(1) {
        Rr = R.coro_call(&R); // process
        if( Rr!=1 ) break; // normally waits for more input

        finp.pop();
        byte* p;
        while( (p=finp.inp())==0 ) wait();
        R.addinp( p, inpbufsize ); 
      }
      tmp.push(); // add the new buffer

      if( Rr==0 ) break;
    }
  }

};

thread_dec th_dec;

void processfile1( filehandle f, filehandle g, uint _f_len ) {

  tmp.init();
  SetThreadAffinityMask( GetCurrentThread(), 1 );

  finp.init(); finp.a--;
  th_inp.f = f;
  th_inp.start();

  fout.init();
  th_out.g = g;
  th_out.start();

  rle_class1* R1 = mAlloc<rle_class1>(1); rle_class1& R = *R1;

  th_dec.pR = &R;
  th_dec.f = f;
  th_dec.start();

  Model1* M1 = mAlloc<Model1>(1); Model1& M = *M1;

  M.f_len = _f_len;
  M.coro_init();
//  M.addout( outbuf, outbufsize );
  M.addout( fout.out(), outbufsize );
  tmp.a--;

  while( 1 ) {
    int Mr = M.coro_call(&M);
    if( Mr==2 ) {

      fout.setsize( (byte*)M.outbeg, outbufsize ); fout.push(); // allow to write the prev buffer
      byte* p;
      while( (p=fout.out())==0 ) th_out.wait();
      M.addout( p, outbufsize ); // next buffer

    } else { // 2 and 0

      tmp.pop(); // discard the previously processed buffer

      byte* p;
      while( (p=tmp.inp())==0 ) th_dec.wait();
      M.addinp( p, 0 ); // add next buffer to process

      if( Mr==0 ) break;
    }
  }

  fout.setsize( (byte*)M.outbeg, (M.outptr-M.outbeg)|(1<<31) ); fout.push();

  th_dec.quit();
  th_out.quit();
}
