#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

/* needs to stay the first line */
asm("jmp $0, $main");

int readCharacter() {
	int result;
	asm(
		"mov $0x0, %%ah;"
		"int $0x16;"
		: "=a"(result)
	);

	asm(
		"movb $0x04, %ah;"
		"int $0x16;"
	);
	return result;
}

/* Print char */
void printChar(char a) {
	asm(
		"mov $0x0E, %%ah;"
		"int $0x10;"
		:: "a"(a)
	);
}


void restart() {
	asm(
		"int $0x19;"
	);
}
/* space for additional code */

void main(void)
{
	printChar('H');
	printChar('e');
	printChar('l');
	printChar('l');
	printChar('o');
	printChar('!');
	printChar('\r');
	printChar('\n');

	while(1) {
		char pass[8];
		short i;
		for(i = 0; i<8;i++) {
			char input = readCharacter();
			if(input == 0x0d) {
				break;
			}
			pass[i] = input;
			printChar('.');
		}
		printChar('\r');
		printChar('\n');
		if(i==0) {
			restart();
		}
		for(int j=0;j<i;j++) {
			printChar(pass[j]);
		}
		printChar('\r');
		printChar('\n');
	}
//	asm("jmp .");
}
