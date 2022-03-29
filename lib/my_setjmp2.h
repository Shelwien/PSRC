
struct my_jmpbuf {
  uint eip,ebp,esp;
};

#define ASM __asm__ volatile

int __inline my_setjmp( my_jmpbuf* regs ) {
  int r;
  int r0,r1,r2,r3,r4,r5;

  ASM ("\
  xchg %%esp,%%eax; \
  addl $4,%%esp; \
  call 1f; \
1:   addl $15,0(%%esp); \
  movl %%ebp,4(%%esp); \
  movl %%eax,8(%%esp); \
  xchg %%esp,%%eax; \
  xorl %%eax,%%eax; \
  " : "=a"(r),"=b"(r0),"=c"(r1),"=d"(r2),"=S"(r3),"=D"(r4) : "a"(regs) : 
  );


  return r;
}

//__declspec(noreturn) 
void __inline my_jmp( my_jmpbuf* regs, int ) {
  ASM ("\
  mov 4(%%ebx),%%ebp; \
  mov 8(%%ebx),%%esp; \
  mov $1,%%eax; \
  jmp *0(%%ebx); \
  " :  : "b"(regs) : 
  );

}

typedef my_jmpbuf m_jmp_buf[1];
#define jmp_buf m_jmp_buf
#define longjmp my_jmp
#define setjmp  my_setjmp

