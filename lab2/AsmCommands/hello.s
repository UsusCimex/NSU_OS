.data
msg:
  .ascii "Hello, world!\n"
  .set len, . - msg

.text

.globl _start
_start:
  # write
  mov  $1,   %rax
  mov  $1,   %rdi
  mov  $msg, %rsi
  mov  $len, %rdx
  syscall

  # exit
  mov  $60, %rax
  xor  %rdi, %rdi
  syscall