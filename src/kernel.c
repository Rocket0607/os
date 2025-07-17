#include "../include/heap.h"
#include "../include/keymap.h"

#define IDT_SIZE 256
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_DATA_PORT 0x60


extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

struct IDT_Entry {
    unsigned short int offset_lower;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higher;
};

int cursor = 0;
char *vidmem = (char*)0xb8000; // set value for where video memory begins
struct IDT_Entry IDT[IDT_SIZE];


void idt_init(void)
{
    unsigned long keyboard_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    keyboard_address = (unsigned long)keyboard_handler; // keyboard_handler defined in asm
    IDT[0x21].offset_lower = keyboard_address & 0xffff;
	IDT[0x21].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[0x21].offset_higher = (keyboard_address & 0xffff0000) >> 16;

    /* Ports for PIC's
	*	        PIC1	PIC2
	*Command    0x20	0xA0
	*Data	    0x21	0xA1
	*/
    	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);  
	write_port(0xA1 , 0x00);  

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

    // descriptor has to contain size of IDT and its address
    // idt_ptr is simply used to pack the values (struct could also be used)
    idt_address = (unsigned long)IDT;
    // first index has the size in the first two bytes, then the first 2 bytes of the 
    //     address and the second index has the other 2 bytes of the address 
    idt_ptr[0] = (sizeof(struct IDT_Entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    load_idt(idt_ptr);
}

void kb_init(void)
{
    write_port(0x21, 0xFD); // writes 11111101, enabling only IRQ1 which is what we want for the keyboard
}

void keyboard_handler_main(void)
{
    unsigned char status; // status of keyboard indicates whether or not something is being pressed
    char keycode; // holds the actual keycode of key being pressed

    write_port(0x20, 0x20); // End Of Interrupt, telling the PIC that it can take further interrupts (now from the keybpboard)

    status = read_port(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        keycode = read_port(KEYBOARD_DATA_PORT);
        if (keycode < 0) return;
        if (keymap[(unsigned char) keycode] == '\n') {
            handle_command();
        } else {
            insert_char_at_cursor(keymap[(unsigned char) keycode]);
        }
    }
}

void clear_screen(void)
{
    // video memory sets display values for each section of the screen
    // loop through video memory and clear all values
    // 25 lines, 80 columns with each section using 2 bytes, one for the character and one attribute byte
    for (int i = 0; i < 80 * 25 * 2; i+=2) {
        // empty out characters
        vidmem[i] = ' ';
        // sets colour to light-grey on black screen
        vidmem[i+1] = 0x07;
    }
    cursor = 0;
}

void insert_str_at_cursor(char* str)
{
    unsigned int j = 0;
    while (str[j] != '\0') {
        // set character byte using ascii
        vidmem[cursor] = str[j];
        vidmem[cursor+1] = 0x07;
        ++j;
        cursor += 2;
    }
}

void insert_char_at_cursor(char chr)
{
   vidmem[cursor++] = chr;
   vidmem[cursor++] = 0x07; 
}

void insert_newline_at_cursor()
{
    cursor = cursor + (160 - (cursor % 160));
}

void display_new_prompt()
{
    insert_newline_at_cursor();
    insert_newline_at_cursor();
    insert_char_at_cursor('$');
    insert_char_at_cursor(' ');
}

void handle_command()
{
    const int command_len = cursor % 160 - 2;
    char command[command_len/2];
    for (int i = 0; i < command_len/2; ++i) {
        command[i] = vidmem[cursor-command_len+2+i*2];
    }
    insert_newline_at_cursor();
    insert_str_at_cursor(command);
    display_new_prompt();
}

void heap_test(char *a)
{
    vidmem[0] = *a;
}

void kmain(void)
{
    clear_screen();
    insert_char_at_cursor('$');
    insert_char_at_cursor(' ');
    idt_init();
    kb_init();

    init_heap();

    char *a = (char *)alloc(1);
    *a = 'a';
    heap_test(a);
    free(a);
    while(1);
}