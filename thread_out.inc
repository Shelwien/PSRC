
struct thread_out : thread<thread_out> {

  filehandle g;

  void thread( void ) {
    SetThreadAffinityMask( GetCurrentThread(), 4 );

    while(1) {

      byte* p;
      while( (p=fout.inp())==0 ) wait(); // wait for free buffer

      uint l = fout.getsize(p);
      uint f_EOF = (l&(1<<31))>0;
      l &= (1U<<31)-1;
      if( l==0 ) break;
      g.writ( p, l ); // write data from buffer

//printf( "th_out: %08X %i %i\n", p, l, f_EOF );

      if( f_EOF ) break;

      fout.pop();
    }

  }

};

thread_out th_out;
