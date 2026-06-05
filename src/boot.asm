; B22-OS Bootloader
; Simple x86 32-bit bootloader using GRUB

; Declare constants for the multiboot header
MBHEADER_FLAGS = 0x0
MBHEADER_MAGIC = 0x1BADB002
MBHEADER_CHECKSUM = -(MBHEADER_MAGIC + MBHDR_FLAGS)

; Multiboot header
section .multiboot
align 4
    dd MBHEADER_MAGIC
    dd MBHEADER_FLAGS
    dd MBHEADER_CHECKSUM

; Reserve space for a simple stack
section .bss
align 16
stack_bottom:
resb 16384  ; 16 KiB stack
stack_top:

; Kernel entry point
section .text
global _start:function (_start.end - _start)
_start:
    ; Setup stack
    mov esp, stack_top

    ; Push multiboot info pointer (from bootloader in ebx)
    push ebx
    
    ; Push multiboot magic number
    push eax
    
    ; Call kernel main function
    extern kernel_main
    call kernel_main
    
    ; Halt the CPU if kernel returns
    cli
.hang:
    hlt
    jmp .hang
.end:
