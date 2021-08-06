ldr: *.c
	gcc -nostdlib -nostdinc -isystem ../freebsd-headers -D main=ldr_main -DPREFIX='"/user/app/BREW00179/hamachi"' -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 -DPAGE_SIZE=16384 *.c -Wl,-r -o ldr --std=gnu99 -g -pthread -no-pie -fPIE

clean:
	rm -f ldr

all: ldr
