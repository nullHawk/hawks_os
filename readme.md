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

## Screenshots:
![Screenshot 2025-04-16 111350](https://github.com/user-attachments/assets/ec6bc57a-3cb7-4a75-a6e7-f709021ee72d)
