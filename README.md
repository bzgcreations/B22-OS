# B22-OS

A very simple, minimal operating system built from scratch.

## Features

- ✅ Custom bootloader (32-bit)
- ✅ Simple kernel written in C
- ✅ Text-mode display output
- ✅ Memory management basics
- ✅ Keyboard input handling
- ✅ Simple command interpreter
- ✅ Bootable via GRUB

## Requirements

- `nasm` - Netwide Assembler
- `gcc` - GNU C Compiler (with 32-bit support)
- `ld` - GNU Linker
- `qemu-system-i386` - QEMU x86 emulator
- `grub-mkrescue` - GRUB bootloader generator
- `xorriso` - ISO 9660 image creation

### Install on Ubuntu/Debian

```bash
sudo apt-get install build-essential nasm qemu-system-x86 grub-pc-bin xorriso
```

### Install on macOS (with Homebrew)

```bash
brew install nasm qemu grub xorriso
```

## Building

```bash
# Build and create ISO
make iso

# Run in QEMU emulator
make run

# Clean build files
make clean
```

## Project Structure

```
B22-OS/
├── Makefile              # Build configuration
├── README.md             # This file
├── src/
│   ├── boot.asm         # Bootloader (x86 assembly)
│   ├── kernel.c         # Kernel main code (C)
│   ├── kernel.h         # Kernel header files
│   ├── linker.ld        # Linker script
│   └── grub.cfg         # GRUB bootloader config
└── bin/
    └── B22-OS.iso       # Bootable ISO image (generated)
```

## How It Works

1. **Bootloader** (boot.asm)
   - Initializes the CPU and memory
   - Sets up the stack
   - Calls the kernel main function

2. **Kernel** (kernel.c)
   - Initializes the VGA text buffer
   - Displays boot messages
   - Handles keyboard input
   - Provides basic command interpreter

3. **Build Process**
   - Assemble bootloader to object file
   - Compile kernel C code
   - Link both into a single binary
   - Wrap in GRUB bootloader
   - Create ISO image for booting

## Commands

When the OS boots, you can type commands:

- `help` - Show available commands
- `clear` - Clear the screen
- `info` - Show system information
- `echo <text>` - Print text
- `halt` - Shut down the OS

## Running on Real Hardware

To run on real hardware:

1. Write the ISO to a USB stick:
   ```bash
   sudo dd if=bin/B22-OS.iso of=/dev/sdX bs=4M
   ```
   (Replace `/dev/sdX` with your USB device)

2. Boot from the USB stick

## Architecture

- **CPU Mode**: 32-bit protected mode
- **Bootloader**: GRUB 2
- **Kernel**: Monolithic
- **Display**: VGA text mode (80x25)
- **Memory Model**: Flat memory model

## Future Enhancements

- [ ] Interrupt handling (IDT, GDT)
- [ ] Paging and virtual memory
- [ ] Process management
- [ ] Filesystem support
- [ ] Networking
- [ ] More comprehensive shell
- [ ] Device drivers

## License

MIT License - Feel free to modify and learn!

## Resources

- [OSDev.org](https://wiki.osdev.org/) - OS Development Wiki
- [Bare Metal x86 Programming](https://www.codeproject.com/Articles/45788/The-Real-Mode-x86-Interrupt-Vector-Table)
- [GRUB Manual](https://www.gnu.org/software/grub/manual/)
- [Intel x86 Documentation](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
