
struct thread_inp : thread<thread_inp> {

  filehandle f;

  void thread( void ) {
    SetThreadAffinityMask( GetCurrentThread(), 2 );

    while(1) {

      byte* p;
      while( (p=finp.out())==0 ) wait(); // wait for free buffer

      uint l = f.read( p, inpbufsize ); // read data into buffer

//printf( "th_inp: %08X %i\n", p, l );

      finp.setsize( p, l ); finp.push(); // set size / allow access
      if( l==0 ) break; // get/put don't support repeated failures

    }

  }

};

thread_inp th_inp;
