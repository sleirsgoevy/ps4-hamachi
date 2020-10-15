all: hamachid cpp.elf

clean:
	rm -f hamachi.tgz hamachid cpp.o cpp.elf

hamachi.tgz:
	wget https://vpn.net/installers/logmein-hamachi-2.1.0.203-x64.tgz -O hamachi.tgz

hamachid: hamachi.tgz
	tar -xvzf hamachi.tgz logmein-hamachi-2.1.0.203-x64/hamachid -O > hamachid

cpp.o: cpp.txt
	gcc -no-pie -x c cpp.txt -nostdlib -static -Wl,-r -lstdc++ -nostartfiles -o cpp.o

cpp.elf: cpp.o
	gcc -no-pie cpp.o -Ttext=0x60000000 -Wl,-section-start=.interp=0x61000000 -nostartfiles -o cpp.elf
