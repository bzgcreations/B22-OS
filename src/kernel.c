#include "kernel.h"
#include <stdarg.h>

/* VGA State */
static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;
static size_t vga_row = 0;
static size_t vga_col = 0;
static uint8_t vga_color = (VGA_COLOR_LIGHT_GREY << 4) | VGA_COLOR_BLACK;

/* Keyboard Buffer */
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static size_t keyboard_index = 0;

/* ===== Low-level I/O Functions ===== */

uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %w1, %b0" : "=a"(value) : "d"(port));
    return value;
}

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %b0, %w1" : : "a"(value), "d"(port));
}

/* ===== VGA Display Functions ===== */

void vga_init(void) {
    vga_buffer = (uint16_t *)VGA_MEMORY;
    vga_row = 0;
    vga_col = 0;
    vga_color = (VGA_COLOR_BLACK << 4) | VGA_COLOR_LIGHT_GREY;
    vga_clear();
}

void vga_clear(void) {
    uint16_t blank = ' ' | (vga_color << 8);
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    vga_row = 0;
    vga_col = 0;
}

void vga_scroll(void) {
    if (vga_row < VGA_HEIGHT) return;
    
    uint16_t blank = ' ' | (vga_color << 8);
    for (size_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }
    
    for (size_t i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = blank;
    }
    
    vga_row = VGA_HEIGHT - 1;
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
    } else if (c == '\r') {
        vga_col = 0;
    } else if (c == '\t') {
        vga_col += 4;
    } else if (c >= 32 && c <= 126) {
        size_t index = vga_row * VGA_WIDTH + vga_col;
        vga_buffer[index] = c | (vga_color << 8);
        vga_col++;
    }
    
    if (vga_col >= VGA_WIDTH) {
        vga_col = 0;
        vga_row++;
    }
    
    vga_scroll();
}

void vga_write_string(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

void vga_set_color(uint8_t foreground, uint8_t background) {
    vga_color = (background << 4) | foreground;
}

void vga_reset_color(void) {
    vga_color = (VGA_COLOR_BLACK << 4) | VGA_COLOR_LIGHT_GREY;
}

/* ===== Kernel Output Functions ===== */

void kernel_putc(char c) {
    vga_putchar(c);
}

void kernel_puts(const char *str) {
    vga_write_string(str);
    vga_putchar('\n');
}

void kernel_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    int val = va_arg(args, int);
                    char buf[20];
                    itoa(val, buf, 10);
                    vga_write_string(buf);
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    char buf[20];
                    itoa(val, buf, 16);
                    vga_write_string(buf);
                    break;
                }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    vga_write_string(str);
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    vga_putchar(c);
                    break;
                }
                case '%':
                    vga_putchar('%');
                    break;
            }
        } else if (*fmt == '\\' && *(fmt + 1) == 'n') {
            vga_putchar('\n');
            fmt++;
        } else {
            vga_putchar(*fmt);
        }
        fmt++;
    }
    
    va_end(args);
}

/* ===== String Utilities ===== */

void itoa(int value, char *str, int base) {
    const char *digits = "0123456789abcdef";
    char buf[33];
    int i = 0;
    int negative = 0;
    
    if (value < 0) {
        negative = 1;
        value = -value;
    }
    
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    while (value > 0) {
        buf[i++] = digits[value % base];
        value /= base;
    }
    
    if (negative) {
        buf[i++] = '-';
    }
    
    int j = 0;
    while (i > 0) {
        str[j++] = buf[--i];
    }
    str[j] = '\0';
}

int strlen(const char *str) {
    int len = 0;
    while (*str++) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

void strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

/* ===== Shell Commands ===== */

void cmd_help(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    kernel_puts("B22-OS Command Help");
    vga_reset_color();
    kernel_puts("");
    kernel_puts("Available commands:");
    kernel_puts("  help    - Show this help message");
    kernel_puts("  clear   - Clear the screen");
    kernel_puts("  info    - Show system information");
    kernel_puts("  echo    - Print text");
    kernel_puts("  halt    - Shutdown the OS");
}

void cmd_info(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kernel_puts("System Information:");
    vga_reset_color();
    kernel_puts("");
    kernel_puts("OS Name:        B22-OS");
    kernel_puts("Version:        1.0.0");
    kernel_puts("Architecture:   x86 32-bit");
    kernel_puts("Display:        VGA Text Mode (80x25)");
    kernel_puts("Memory Model:   Flat");
}

void cmd_echo(const char *args) {
    while (*args && *args == ' ') args++;
    kernel_puts(args);
}

void shell_process_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "clear") == 0) {
        vga_clear();
    } else if (strcmp(cmd, "info") == 0) {
        cmd_info();
    } else if (strcmp(cmd, "halt") == 0) {
        kernel_puts("System halting...");
        halt();
    } else if (strcmp(cmd, "") == 0) {
        // Empty command, do nothing
    } else if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o') {
        cmd_echo(cmd + 4);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        kernel_printf("Unknown command: %s\n", cmd);
        vga_reset_color();
    }
}

void shell_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kernel_puts("\nB22-OS> ");
    vga_reset_color();
}

void halt(void) {
    asm volatile("cli; hlt;");
    while (1);
}

/* ===== Main Kernel Function ===== */

void kernel_main(uint32_t magic, void *mbi) {
    vga_init();
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    kernel_puts("========================================");
    kernel_puts("      Welcome to B22-OS v1.0.0");
    kernel_puts("========================================");
    vga_reset_color();
    
    kernel_puts("");
    kernel_puts("Boot successful! Type 'help' for commands.");
    kernel_puts("");
    
    shell_prompt();
    
    // Simple command loop
    char command_buffer[256];
    int cmd_len = 0;
    
    while (1) {
        // In a real OS, we'd have proper interrupt-driven keyboard input
        // For now, we'll just accept typed commands
        // This is a simplified shell for demonstration
        halt();  // Prevent busy-waiting
    }
}
