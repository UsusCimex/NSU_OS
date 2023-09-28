#include <sys/syscall.h>
#include <unistd.h>
 
ssize_t printHello(int fd, const void* buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
}