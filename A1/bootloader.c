#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

/* needs to stay the first line */
asm("jmp $0, $main;");

/* space for additional code */
void printCharacter(char c)
{
	asm(
		"mov $0x0E, %%ah;"
		"int $0x10;"
		:: "a"(c)
	);
}

void printLine(char * toPrint)
{
    while (*toPrint)
        printCharacter(*(toPrint++));
    
    printCharacter('\n');
    printCharacter('\r');
}

char readCharacter(void)
{
    char result;
	asm volatile(
		"mov $0x0, %%ah;"
		"int $0x16;"
        "movb $0x04, %%ah;"
		"int $0x16;"
		: "=a"(result)
	);
	return result;
}

int readPassword(char * buffer, int bufferLength)
{
    int passwordLength = 0;
    char c;
    while (passwordLength < bufferLength - 1)
    {
        c = readCharacter();
        if (c == '\r')
            break;
        else
        {
            printCharacter('.');
            buffer[passwordLength++] = c;
        }
    }
    buffer[passwordLength] = '\0';
    return passwordLength;
}

void reboot(void)
{
    asm("int $0x19;");
}

void main(void)
{
    #define BUFFER_LENGTH 9 // the last byte is used for zero termination
    char password[BUFFER_LENGTH];
    printLine("Hallo!");
    for (;;)
    {
        if (!readPassword(password, BUFFER_LENGTH))
        {
            printLine("Reboot!");
            reboot();
        }
        printLine("");
        printLine(password);
    }
}
