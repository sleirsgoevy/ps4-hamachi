all: tunldr.o

crt.o: crt.asm
	yasm -f elf64 crt.asm

crt-ps4-%.o: crt-ps4-%.asm
	yasm -f elf64 crt-ps4-$*.asm

blob.elf: main.c crt.o
	gcc -D_KERNEL -isystem ../freebsd-headers -nostdinc -nostdlib -static crt.o main.c -o blob.elf -ffreestanding -fPIE -mno-red-zone -g

blob.bin: blob.elf
	objcopy blob.elf --only-section .text --only-section .data --only-section .bss --only-section .rodata -O binary blob.bin

blob-ps4-%.elf: main.c crt-ps4-%.o
	gcc -DPS4 -D_KERNEL -DFIRMWARE=$* -isystem ../freebsd-headers -nostdinc -nostdlib -static crt-ps4-$*.o main.c -o blob-ps4-$*.elf -ffreestanding -fPIE -mno-red-zone -g

blob-ps4-%.bin: blob-ps4-%.elf
	objcopy blob-ps4-$*.elf --only-section .text --only-section .data --only-section .bss --only-section .rodata -O binary blob-ps4-$*.bin

tunldr.o: tunldr.c blob-ps4-505.bin blob-ps4-505.elf blob-ps4-672.bin blob-ps4-672.elf blob-ps4-702.bin blob-ps4-702.elf blob-ps4-755.bin blob-ps4-755.elf
	gcc tunldr.c -ffreestanding -c -o tunldr.o -fPIE

clean:
	rm -f crt.o crt-ps4-*.o blob.elf blob.bin blob-ps4-*.elf blob-ps4-*.bin tunldr.o
