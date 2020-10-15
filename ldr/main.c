#include <signal.h>
#include <unistd.h>

struct symbol_desc;

int load_elf(const char* path, unsigned long long* entry);
void setauxsymbols(struct symbol_desc* data);

int my_argc;
const char** my_argv;

int main(int argc, const char** argv)
{
    signal(SIGPIPE, SIG_IGN);
    my_argc = argc - 3;
    my_argv = argv + 3;
    unsigned long long cpp_syms;
    int q = load_elf(argv[2], &cpp_syms);
    if(q)
        return q;
    setauxsymbols((struct symbol_desc*)cpp_syms);
    unsigned long long entry;
    q = load_elf(argv[1], &entry);
    if(q)
        return q;
    ((void(*)(void))entry)();
    return 0;
}
