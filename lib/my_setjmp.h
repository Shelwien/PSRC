
struct my_jmpbuf {
  union {
    uint regs[9]; 
    struct {
      uint edi; uint esi; uint ebp; uint esp;
      union { uint ebx; word bx; struct { byte bl; byte bh; }; };
      union { uint edx; word dx; struct { byte dl; byte dh; }; };
      union { uint ecx; word cx; struct { byte cl; byte ch; }; };
      union { uint eax; word ax; struct { byte al; byte ah; }; };
      uint eip;
    };
  };
};

//uint r_esp;

#define ASM __asm__ volatile

int __inline my_setjmp( my_jmpbuf* regs ) {
  int r;

//__asm int 3
  ASM ("\
  xchg %%eax,%%esp; \
  add $36,%%esp; \
  call 1f; \
1:   .byte 0x60; \
  add $16,%%esp; \
  pushl %%eax; \
  addl $11,20(%%esp); \
  xchg %%eax,%%esp; \
  mov $0,%%eax; \
  setz %%al; \
  " : "=a"(r) : "a"(regs) : // "ebx","edx","esi","edi","ebp"
  );

  return r;
}

//__declspec(noreturn) 
void __inline my_jmp( my_jmpbuf* regs, int ) {

//__asm int 3
  ASM ("\
  xchg %%eax,%%esp; \
  .byte 0x61; \
  popl %%eax; \
  mov -24(%%esp),%%esp; \
  cmp %%esp,%%esp; \
  jmp *%%eax; \
  " : : "a"(regs) : 
  );
}

typedef my_jmpbuf m_jmp_buf[1];
#define jmp_buf m_jmp_buf
#define longjmp my_jmp
#define setjmp  my_setjmp

