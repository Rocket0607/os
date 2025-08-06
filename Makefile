main:
	nasm -g  -f elf32 src/kernel.asm -o build/kasm.o
	gcc -g -m32 -ffreestanding -fno-stack-protector -c src/kernel.c -o build/kernel.o
	gcc -g -m32 -ffreestanding -fno-stack-protector -c src/heap.c -o build/heap.o
	gcc -g -m32 -ffreestanding -fno-stack-protector -c src/gdt.c -o build/gdt.o
	ld -m elf_i386 -T link.ld -o build/kernel build/kasm.o build/kernel.o build/heap.o build/gdt.o

debug:
	"/mnt/c/Program Files/qemu/qemu-system-i386.exe" -s -S -kernel build/kernel

run:
	"/mnt/c/Program Files/qemu/qemu-system-i386.exe" -kernel build/kernel