#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

/* needs to stay the first line */
asm("jmp $0, $main;");

/* space for additional code */
void printCharacter(char c)
{
    asm volatile("push %ax;");
    asm volatile("push %bx;");
    asm volatile(
        "int $0x10;"
        :: "ax" (0x0e00 | c), "bx"(0x0000)
    );
    asm volatile("pop %bx;");
    asm volatile("pop %ax;");
}

void printLine(char * toPrint)
{
    while (*toPrint)
        printCharacter(*(toPrint++));
    
    printCharacter('\n');
    printCharacter('\r');
}

char getCharacter(void)
{
    char c;
    asm volatile("push %ax;");
    asm volatile("push %bx;");
    asm volatile(
        "int $0x16;"
        :"=al"(c)
        :"ah"(0x00)
    );
    asm volatile("pop %bx;");
    asm volatile("pop %ax;");
    return c;
}

void getLine(char * buffer, int bufferLength)
{
    int length = 0;
    char c;
    while (length < bufferLength - 1)
    {
        c = getCharacter();
        if (c == '\r')
            break;
        else
        {
            printCharacter(c);
            buffer[length++] = c;
        }   
    }
    buffer[length] = '\0';
}

void rebootSystem(void)
{
    asm("int $0x19;"); 
}

void main(void)
{
    #define BUFFER_LENGTH 10
    char input[BUFFER_LENGTH];
    
    printLine("Hallo World!");
    getLine(input, BUFFER_LENGTH);
    printLine("");
    printLine(input);

    while (1);
}
