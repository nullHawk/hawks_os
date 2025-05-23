#include "extra.h"

#define PIC1_C 0x20 // Command port for the master PIC
#define PIC1_D 0x21 // Data port for the master PIC
#define PIC2_C 0xa0 // Command port for the slave PIC
#define PIC2_D 0xa1 // Data Port for the slave PIC

#define ICW1_DEF 0x10
#define ICW1_ICW4 0x01
#define ICW4_x86 0x01

void cls();
void setMonitorColor(char);
void disableBiosCursor();

void printString(char*);
void printChar(char);

void scroll();

void printColorString(char* , char);
void printColorChar(char, char);

void getDecAscii(int);

void initIDT();
extern void _loadIdt();
extern void _isr1_Handler();
void handleKeypress(int);
void pressed(char);
void picRemap();

unsigned char inportb(unsigned short);
void outportb(unsigned short, unsigned char);

char* TM_START;
char NumberAscii[10];
int CELL;

char COMMAND[21];
int i = 0;

struct IDT_ENTRY{
	unsigned short base_Lower;
	unsigned short selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short base_Higher;
};

struct IDT_ENTRY _idt[256];
extern unsigned int _isr1;
unsigned int base;

int _start(){
    TM_START = (char*) 0xb8000;
    CELL = 0;
	base = (unsigned int)&_isr1;

    cls();
    setMonitorColor(0x1F);
	disableBiosCursor();

    char Welcome[] = "Welcome to HawksOS : Copyright 2025\n";
    char Tagline[] = "Command Line Version 0.1\n\n";
    char Prompt[] = "HawksOS> ";

    printColorString(Welcome, 0x1B);
    printColorString(Tagline, 0x1B);
    printColorString(Prompt, 0x1E);

	initIDT();
}

void disableBiosCursor() {
    // Set cursor position to a high value to hide the cursor
    outportb(0x3D4, 0x0A);   // Command to set start scan line
    outportb(0x3D5, 0x20);   // Set start scan line to 0x20 (max position)
    outportb(0x3D4, 0x0B);   // Command to set end scan line
    outportb(0x3D5, 0x20);   // Set end scan line to 0x20 (max position)
}

void cls(){
    int i = 0;
    CELL = 0;
    while(i < (2 * 80 * 25)){
        *(TM_START + i) = ' '; // Clear screen
        i += 2;
    }
}

void setMonitorColor(char Color){
	int i = 1;
	while(i < (2 * 80 * 25)){
		*(TM_START + i) = Color;
		i += 2;
	}
}

void printString(char* cA){
	int i = 0;
	while(*(cA + i) != '\0'){
		printChar(*(cA + i));
		i++;
	}
}

void printChar(char c){
	if(CELL == 2 * 80 * 25)
		scroll();
	if(c == '\n'){
		CELL = ((CELL + 160) - (CELL % 160));
		return;
	}
	*(TM_START + CELL) = c;
	CELL += 2;	
}

void scroll(){
	int i = 160 , y = 0;
	while(i < 2 * 80 * 25){
		*(TM_START + y) = *(TM_START + i);
		i += 2;
		y += 2;
	}
	CELL = 2 * 80 * 24;
	i = 0;
	while(i < 160){
		*(TM_START + CELL + i) = ' ';
		i += 2;
	}
}

void printColorString(char* c , char co){
	int i = 0;
	while(*(c + i) != '\0'){
		printColorChar(*(c + i) , co);
		i++;
	}
}

void printColorChar(char c , char co){
	if(CELL == 2 * 80 * 25)
		scroll();
	if(c == '\n'){
		CELL = ((CELL + 160) - (CELL % 160));
		return;
	}
	*(TM_START + CELL) = c;
	*(TM_START + CELL + 1) = co;
	CELL += 2;	
}

void getDecAscii(int num){
    if(num == 0){
        NumberAscii[0] = '0';
        return;
    }
    char NUM[10];
    int i = 0, j = 0;
    while(num > 0){
        NUM[i] = num % 10;
        num /= 10;
        i++;
    }
    i--;
    while(i >= 0){
        NumberAscii[j] = NUM[i];
        i--;
        j++;
    }
    NumberAscii[j] = 'J';
    j = 0;
    while(NumberAscii[j] != 'J'){
        NumberAscii[j] = '0' + NumberAscii[j];
        j++;
    }
    NumberAscii[j] = 0;
}

void initIDT(){
	// Set the lower 16 bits of the ISR1 handler address in the IDT entry
	_idt[1].base_Lower = (base & 0xFFFF);

	// Set the higher 16 bits of the ISR1 handler address in the IDT entry
	_idt[1].base_Higher = (base >> 16) & 0xFFFF;

	// Set the code segment selector for this IDT entry (0x08 is the kernel code segment)
	_idt[1].selector = 0x08;

	// Reserved field, must be zero
	_idt[1].zero = 0;

	// Set the flags for this IDT entry (0x8e = present, privilege level 0, 32-bit interrupt gate)
	_idt[1].flags = 0x8e;

	// Remap the PIC (Programmable Interrupt Controller) to avoid conflicts with CPU exceptions
	picRemap();

	// Enable IRQ1 (keyboard interrupt) by unmasking it in the PIC
	outportb(0x21, 0xfd); // Mask all IRQs except IRQ1 on PIC1
	outportb(0xa1, 0xff); // Mask all IRQs on PIC2

	// Load the IDT (Interrupt Descriptor Table) into the CPU
	_loadIdt();
}

unsigned char inportb(unsigned short _port){
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(unsigned short _port, unsigned char _data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

extern void _isr1_Handler(){
	handleKeypress(inportb(0x60));
	outportb(0x20 , 0x20);
	outportb(0xa0 , 0x20);
}

void handleKeypress(int code){
	char OSM[] = "\nHawksOS> ";
	char Scancode[] = {
        0 , 8 , '1' , '2' , '3' , '4' , '5' , '6' ,
        '7' , '8' , '9' , '0' , '-', '=', 0 , 0 , 
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
        'O', 'P', '[', ']', 0 , 0 , 'A', 'S', 'D', 
        'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 
        0 , '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
        ',', '.', '/', 0 , '*', 0 , ' '
    };
	
	if(code == 0x1c){
		COMMAND[i] = '\0';
		i = 0;
		strEval(COMMAND);
		printColorString(OSM, 0x1E);
	}  else if(code == 0x0E) {  // Backspace pressed
		if(i > 0) {
			i--;
			CELL -= 2;  // Move cursor back
			// Erase character
			*(TM_START + CELL) = ' ';
			*(TM_START + CELL + 1) = 0x1F;  // Maintain prompt color
		}
	} else if(code < 0x3a){
		char key = Scancode[code];
        if(key == 8) return;  // Already handled backspace
        pressed(key);
	}
	outportb(0x20, 0x20);
    outportb(0xa0, 0x20);
}

void pressed(char key){
	COMMAND[i] = key;
	i++;
	printChar(key);
}

// Remaps IRQ so that it doesn't overlap with CPU executions
void picRemap(){
	unsigned char a , b;
	a = inportb(PIC1_D);
	b = inportb(PIC2_D);

	outportb(PIC1_C , ICW1_DEF | ICW1_ICW4);
	outportb(PIC2_C , ICW1_DEF | ICW1_ICW4);

	outportb(PIC1_D , 0);
	outportb(PIC2_D , 8);

	outportb(PIC1_D , 4);
	outportb(PIC2_D , 2);

	outportb(PIC1_D , ICW4_x86);
	outportb(PIC2_D , ICW4_x86);

	outportb(PIC1_D , a);
	outportb(PIC2_D , b);
}