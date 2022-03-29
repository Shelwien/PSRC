
#pragma once

//#define FILE_BEGIN           0
//#define FILE_CURRENT         1
//#define FILE_END             2

#define ffWRITE_THROUGH         0x80000000
#define ffOVERLAPPED            0x40000000
#define ffNO_BUFFERING          0x20000000
#define ffRANDOM_ACCESS         0x10000000
#define ffSEQUENTIAL_SCAN       0x08000000
#define ffDELETE_ON_CLOSE       0x04000000
#define ffBACKUP_SEMANTICS      0x02000000
#define ffPOSIX_SEMANTICS       0x01000000
#define ffOPEN_REPARSE_POINT    0x00200000
#define ffOPEN_NO_RECALL        0x00100000
/*
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

extern "C" {

__declspec(dllimport) int __stdcall 
  CreateFileA(
    char* lpFileName,
    unsigned int  dwDesiredAccess,
    unsigned int  dwShareMode,
    void* lpSecurityAttributes,
    unsigned int  dwCreationDisposition,
    unsigned int  dwFlagsAndAttributes,
    int hTemplateFile
  );

__declspec(dllimport) int __stdcall
  WriteFile(
    int hFile,
    void* lpBuffer,
    unsigned int  nNumberOfBytesToWrite,
    unsigned int* lpNumberOfBytesWritten,
    void* lpOverlapped
  );

__declspec(dllimport) int __stdcall
  ReadFile(
   int hFile,
   void* lpBuffer,
   unsigned int  nNumberOfBytesToRead,
   unsigned int* lpNumberOfBytesRead,
   void* lpOverlapped
  );

__declspec(dllimport) int __stdcall 
  CloseHandle( int hObject );

__declspec(dllimport) unsigned int  __stdcall
  SetFilePointer(
    int hFile,
    unsigned int  lDistanceToMove,
    unsigned int* lpDistanceToMoveHigh,
    unsigned int  dwMoveMethod
  );

}
*/

// Log = Open( PswLog, OPEN_ALWAYS, ffWRITE_THROUGH );
HANDLE Win32_Open( const wchar_t* s, uint Flags, uint Attrs ) {

   return CreateFileW( 
     s,
     GENERIC_READ | GENERIC_WRITE,
     0, //FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
     0, // security
     Flags,
     Attrs,
     0
   );

}


HANDLE file_opendir( const wchar_t* s ) {
  HANDLE r = CreateFileW( 
     s,
     GENERIC_WRITE,
     FILE_SHARE_READ | FILE_SHARE_WRITE, //FILE_SHARE_DELETE,
     0, // security
     OPEN_EXISTING,
     FILE_FLAG_BACKUP_SEMANTICS,
     0
   );
  return r!=INVALID_HANDLE_VALUE ? r : 0;
}

int file_mkdir( const wchar_t* name ) {
  return CreateDirectoryW( name, 0 );
}

HANDLE file_open( const wchar_t* name ) { 
  HANDLE r = CreateFileW( 
     name,
     GENERIC_READ,
     FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
     0, // security
     OPEN_EXISTING,
     0,
     0
  );
  return r!=INVALID_HANDLE_VALUE ? r : 0;
}

uint file_open_mode = GENERIC_READ;

void file_open_mode_r( void ) { file_open_mode = GENERIC_READ; }
void file_open_mode_rw( void ) { file_open_mode = GENERIC_READ | GENERIC_WRITE; }

HANDLE file_open( const char* name ) { 
  HANDLE r = CreateFileA(
     name,
     file_open_mode,
     FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
     0, // security
     OPEN_EXISTING,
     0,
     0
  );
  return r!=INVALID_HANDLE_VALUE ? r : 0;
}

HANDLE file_make( const wchar_t* name ) { 
  HANDLE r = CreateFileW( 
     name,
     GENERIC_WRITE,
     FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
     0, // security
     CREATE_ALWAYS,
     0,
     0
  );
  return r!=INVALID_HANDLE_VALUE ? r : 0;
}

HANDLE file_make( const char* name ) { 
  HANDLE r = CreateFileA( 
     name,
     GENERIC_WRITE,
     FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
     0, // security
     CREATE_ALWAYS,
     0,
     0
  );
  return r!=INVALID_HANDLE_VALUE ? r : 0;
}

int file_close( HANDLE file ) {
  return CloseHandle( file );
}

uint file_read( HANDLE file, void* _buf, uint len ) {
  byte* buf = (byte*)_buf;
  uint r = 0;
  ReadFile( file, buf, len, (LPDWORD)&r, 0 );
  return r;
}

uint file_writ( HANDLE file, void* _buf, uint len ) {
  byte* buf = (byte*)_buf;
  uint r = 0;
  WriteFile( file, buf, len, (LPDWORD)&r, 0 );
  return r;
}

uint file_tell( HANDLE file ) {
  return SetFilePointer( file, 0, 0, FILE_CURRENT );
}

qword file_seek( HANDLE file, qword ofs, int typ = FILE_BEGIN ) {
  union {
    struct {
      LONG lo;
      LONG hi;
    };
    qword hilo;
  } Ofs; // = { uint(ofs), uint(ofs>>32) };
  Ofs.hilo = ofs;
  Ofs.lo = SetFilePointer( file, Ofs.lo, &Ofs.hi, typ );
//printf( "lo=%08X hi=%08X %I64i\n", Ofs.lo, Ofs.hi, Ofs.hilo );
  return Ofs.hilo;
}

qword file_size( HANDLE file ) {
  qword r = file_seek( file, 0, FILE_END );
  file_seek( file, 0 );
  return r;
}

/*
char* GetErrorText( void ) { 
  wchar_t* lpMsgBuf;
  DWORD dw = GetLastError(); 
  FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&lpMsgBuf, 0, NULL 
  );
  static char out[32768];
  int wl=wcslen(lpMsgBuf);
  WideCharToMultiByte( CP_OEMCP, 0, lpMsgBuf,wl+1, out,sizeof(out), 0,0 );
  LocalFree(lpMsgBuf);
  wl = strlen(out);
  for( --wl; wl>=0; wl-- ) if( (byte&)out[wl]<32 ) out[wl]=' ';
  return out;
}
*/

