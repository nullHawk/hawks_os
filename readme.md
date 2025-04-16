## Tools required:
- `nasm` : To convert assembly code to machine code for x86 architecture
- `qemu` : Emulator to emulate x86 environment and run the bootloader
## Instructions:
```bash
nasm boot-sect0.asm -f bin -o boot.bin    
```
```bash
qemu-system-i386 -drive format=raw,file=boot.bin 
```