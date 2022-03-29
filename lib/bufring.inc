
template< int bufN, int bufsize >
struct t_bufring {

  volatile uint a;
  volatile uint b;
  ALIGN(4096) byte buf[bufN][bufsize];

  void init( void ) { a=bufN; b=bufN; }

//  void fixptr( void ) { uint z = Min(a,b); z-=z%bufN; a-=z; b-=z; }
  void fixptr( void ) { }

  void  pop( void ) { a++; fixptr(); }
  void push( void ) { b++; fixptr(); }

  byte* inp( void ) { fixptr(); return (a>=b) ? 0 : &buf[a%bufN][0]; }
  byte* out( void ) { fixptr(); return (a+bufN<=b) ? 0 : &buf[b%bufN][0]; }

  void setsize( byte* p, uint l ) { ((uint&)p[bufsize-4]) = l; }
  uint getsize( byte* p ) { return ((uint&)p[bufsize-4]); }

};
