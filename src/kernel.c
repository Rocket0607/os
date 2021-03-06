// TODO: how does vga_entry() work?
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

// kinda combines these two numbers (from the enum) into smth that is
// understandable for the computer. For example, if the bg was black and the
// foreground was light grey, that would make the numbers = 0 , 7 = 00000000,
// 00000111, after the bitshift on the black, it is 00000000 and when the
// bitiwser or is performed the binary becomes 00000111, binary for 7. Now if
// you replace vga_entry_color with 7, the screen will show black background
// with light grey text.
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

// Similar to the last one except for the fact that to do this manually, you
// need to convert the char to binary, this can be done with an ascii chart.
// Then the same steps as vga_entry_color are performed. The (unint16_t) is just
// casting the char and the color into that type, like a conversion. (I think
// this is what is happending though i could be wrong)
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

// width and height of the window (os)
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

// sets all the above vars. To set terminal_buffer, it sets it to a certain
// memory address, then loops through and sets all of them to nothing (a space
// char)
void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  /* terminal_color = 7; */
  terminal_buffer = (uint16_t*)0xB8000;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
      /* terminal_buffer[index] = 1896; */
    }
  }
}

void terminal_setcolor(uint8_t color) {
  terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_row++;
    terminal_column = 80;
  } else {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  }
  if (++terminal_column >= VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
      terminal_row = 0;
  }
}

void terminal_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
  terminal_write(data, strlen(data));
}

void kernel_main(void) {
  /* Initialize terminal interface */
  terminal_initialize();

  /* Newline support is left as an exercise. */
  terminal_writestring(
      "Welcome to Rocket OS. You can't do anything right now cuz im "
      "bad.\nCheck back soon to see if i got any better (unlikely).");
}
