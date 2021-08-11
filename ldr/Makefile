ldr: *.c
	gcc -nostdlib -nostdinc -isystem ../freebsd-headers -Dmain=ldr_main -DPREFIX='"/user/home/hamachi"' -DPAGE_SIZE=16384 *.c -Wl,-r -o ldr --std=gnu99 -g -pthread -no-pie -fPIE

clean:
	rm -f ldr

all: ldr
