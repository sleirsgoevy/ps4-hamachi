#include <signal.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <orbis/libkernel.h>
#include <pthread.h>
#include <setjmp.h>

static jmp_buf jb;

__attribute__((naked)) int my_fork()
{
    asm volatile(
        "lea jb(%rip), %rdi\n"
        "jmp setjmp\n"
    );
}

void my_exit(int code)
{
    longjmp(jb, 1);
}
