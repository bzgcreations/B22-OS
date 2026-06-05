#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

/* VGA Text Mode Constants */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

/* VGA Color Codes */
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_YELLOW 14
#define VGA_COLOR_WHITE 15

/* Keyboard Scan Codes */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 256

/* Function Prototypes */
void kernel_main(uint32_t magic, void *mbi);
void vga_init(void);
void vga_putchar(char c);
void vga_write_string(const char *str);
void vga_clear(void);
void vga_set_color(uint8_t foreground, uint8_t background);
void vga_reset_color(void);

void kernel_puts(const char *str);
void kernel_putc(char c);
void kernel_printf(const char *fmt, ...);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);

void halt(void);
void shell_prompt(void);

#endif /* KERNEL_H */
