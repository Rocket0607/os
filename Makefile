main:
	nasm -f elf32 src/kernel.asm -o build/kasm.o
	gcc -m32 -ffreestanding -fno-stack-protector -c src/kernel.c -o build/kernel.o
	gcc -m32 -ffreestanding -fno-stack-protector -c src/heap.c -o build/heap.o
	ld -m elf_i386 -T link.ld -o build/kernel build/kasm.o build/kernel.o build/heap.o

run:
	"/mnt/c/Program Files/qemu/qemu-system-i386.exe" -kernel build/kernel