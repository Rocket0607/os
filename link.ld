/* defines type of output file */
OUTPUT_FORMAT(elf32-i386)
/* defines entry point of program (defined in kernel .asm) */
ENTRY(start)
/* sets sections defined in kernel.asm */
SECTIONS
    {
        . = 0x100000;
        .text : { *(.text) }
        .data : { *(.data) }
        .bss  : { *(.bss)  }
    }