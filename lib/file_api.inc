
#pragma once

#include "file_win.inc"

struct filehandle {

  HANDLE f;

  filehandle() : f(0) {}

  operator int( void ) {
    return int(f);
  }

  template< typename CHAR >
  filehandle( const CHAR* name, uint f_wr=0 ) {
    f_wr ? make(name) : open( name );
  }

  filehandle( int x ) { f=0; }

  template< typename CHAR >
  uint open( const CHAR* name ) {
    f = file_open( name );
    return uint(f);
  }

  template< typename CHAR >
  uint make( const CHAR* name ) {
    f = file_make( name );
    return uint(f);
  }

  int close( void ) {
    return file_close(f);
  }

  qword size( void ) { return file_size(f); }

  void seek( qword ofs, uint typ=FILE_BEGIN ) {
    file_seek( f, ofs, typ );
  }

  template< typename RESULT >
  RESULT tell( void ) {
    return file_tell(f);
  }

  template< typename BUF >
  uint read( BUF& buf ) {
    return read( &buf, sizeof(buf) )!=sizeof(buf);
  }

  uint read( void* _buf, uint len ) {
    return file_read( f, _buf, len );
  }

  int Getc( void ) {
    byte c;
    uint l = read(c);
    return l ? -1 : c;
  }

  uint writ( void* _buf, uint len ) {
    return file_writ( f, _buf, len );
  }

  template< typename BUF >
  uint writ( BUF& buf ) {
    return writ( &buf, sizeof(buf) )!=sizeof(buf);
  }

};
