
template <class child> 
struct thread {

  HANDLE th;

  uint start( void ) {
    th = CreateThread( 0, 0, &thread_w, this, 0, 0 );
    return th!=0;
  }

  void quit( void ) {
    WaitForSingleObject( th, INFINITE );
    CloseHandle( th );
  }

  void wait( void ) { Sleep(10); } // wait until buffer is processed

  // wrappers to redirect static calls into object method calls
  static DWORD WINAPI thread_w( LPVOID lpParameter ) { ((child*)lpParameter)->thread(); return 0; }
};
