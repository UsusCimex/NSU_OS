#include <stdio.h>

int main() {
    const char message[] = "hello world\n";
    size_t len = sizeof(message) - 1;
    int result;
    asm volatile (
        "movl $1, %%eax\n"   // смещение системного вызова write в регистр eax
        "movl $1, %%edi\n"   // стандартный вывод в регистр edi
        "movq %[msg], %%rsi\n" // указатель на сообщение в регистр rsi
        "movq %[len], %%rdx\n" // длина сообщения в регистр rdx
        "syscall\n"          // вызов системного вызова
        "movl %%eax, %0\n"   // сохранение результата в переменную result
        : "=r" (result)      // выходной операнд для сохранения результата
        : [msg] "m" (message), [len] "m" (len)
        : "eax", "edi", "rsi", "rdx", "cc", "memory"
    );
    if (result < 0) {
        perror("write");
        return 1;
    }
    return 0;
}
