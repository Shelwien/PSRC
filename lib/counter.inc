
struct LinearMixer {
  word w;
  LinearMixer( int W=SCALE/2 ) : w(W) {}
  int Mixup( int p1, int p2 ) {
    return p1+(((p2-p1)*w)>>SCALElog);
  }
};

struct Counter {

  word P; 

  void Update( const int c, const int wr, const int Mw ) {
    int dp = P + Mw-SCALE + ((SCALE-2*Mw)&(-c));
    dp = ((dp*wr)>>SCALElog);
    int q = P - dp + c;
    P = q & mSCALE;
  }

//    q = (q*(SCALE-wr) + dp*wr) >> SCALElog;
  void update( const int c, const int wr, const int mw ) {
/*
    const int dp = c ? mw : (SCALE-mw);
    int q = P;
//    q = (q*(SCALE-wr) + dp*wr) / SCALE;
    q = q + (((dp-q)*wr) >> SCALElog);
    P = q & mSCALE;
*/
//    int dp = P + mw-SCALE + ((SCALE-2*mw)&(-c));

//0: dp = P - (SCALE-mw)
//1: dp = P - mw

    int dp = c ? mw : (SCALE-mw);
    dp = ((P-dp)*wr) >> SCALElog;
    int q = P - dp + c;
    P = q & mSCALE;
  }


};
