nasm boot.asm -f bin -o bin/bootsect.bin
nasm kernel_entry.asm -f elf -o bin/entry.bin

gcc -m32 -ffreestanding -c main.c -o bin/kernel.o

ld -m elf_i386 -Ttext 0x1000 -o bin/kernel.img bin/entry.bin bin/kernel.o  # Added -m elf_i386
objcopy -O binary -j .text bin/kernel.img bin/kernel.bin
cat bin/bootsect.bin bin/kernel.bin > bin/os-image

qemu-system-i386 -drive format=raw,file=bin/os-image