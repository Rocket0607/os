; 32-bit
bits 32

section .text
    ; instructions to follow multiboot spec
    ; requires that data must be aligned to 4 byte boundaries (start at addresses divisible by 4, 0x1004, 0x1008)
    align 4
    ; define doubleword (4 bytes)
    ; store the data so that it can be found when booting
    dd 0x1BADB002
    dd 0x00
    dd - (0x1BADB002 + 0x00)

global start
global read_port
global write_port
global load_idt
global keyboard_handler
global heap_ptr
global fs_ptr
extern kmain ; c function
extern keyboard_handler_main

read_port:
    mov edx, [esp + 4] ; Get address of the port number that was pushed to the stack before the function wsa called
    in al, dx ; Reads the port number given by the address pushed previously (dx is lower 16 bits of edx) and places output in al
    ret

write_port:
    mov edx, [esp + 4] ; Gets memory address of port address
    mov al, [esp + 4 + 4] ; Gets data to write to port
    out dx, al ; writes data in al to port in dx
    ret

load_idt:
    mov edx, [esp + 4]
    lidt [edx] ; required data stored in idt_ptr which is passed into load_idt (size and address packed together)
    sti ; turns interrupts back on
    ret
keyboard_handler:
    call keyboard_handler_main
    iretd ; needs to be used since this is an interrupt handler and we're returning to the program that was interrupted
start:
    cli ; blocks interrupts by clearing the IF flag, informing the processor to ignore maskable external interrupts
    mov esp, stack_space ; defines stack point as stack_space (defined later)
    ; load gdt
    call kmain
    hlt ; halts cpu until next external interrupt

section .bss
heap_ptr: ; sets heap to bottom of reserved memory
resb 16384 ; reserve 16kB
stack_space: ; sets stack_space value used above to top of reserved memory

resb 8192
fs_ptr: