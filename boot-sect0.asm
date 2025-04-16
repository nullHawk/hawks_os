mov ah, 0x0e

mov al , 'H'
int 0x10 ; int is a command in assembly known as Interrupt. This command is used to pause the current execution and execute another code which is defined in a section named as Interrupt DescriptorTable.

mov al, 'E'
int 0x10

mov al, 'L'
int 0x10

mov al, 'L'
int 0x10

mov al, 'O'
int 0x10

mov al, ' '
int 0x10

mov al, 'W'
int 0x10

mov al, 'O'
int 0x10

mov al, 'R'
int 0x10

mov al, 'L'
int 0x10

mov al, 'D'
int 0x10

jmp $

times 510-($-$$) db 0
dw 0xaa55