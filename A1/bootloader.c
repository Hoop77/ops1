#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

#define NULL 0

typedef int bool;
#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE 100

/* needs to stay the first line */
asm("jmp $0, $main");

/* space for additional code */
void printCharacter(char toPrint)
{
    asm(
        "int $0x10;"
        :: "a" (0x0E00 | toPrint)
    );
    return;
}

void printString(char * toPrint)
{
    for (int i = 0; toPrint[i] != '\0'; i++)
        printCharacter(toPrint[i]);
}

void printLine(char * toPrint)
{
    printString(toPrint);
    printCharacter('\n');
}

char getCharacter(void)
{
    char c;
    asm volatile(
        "int $0x10;"
        :"=al"(c)
        :"ah"(0x08)
    );
    return c;
}

void getLine(char * buffer, int bufferLength, bool show, char mask)
{
    int length = 0;
    while (length < bufferLength)
    {
        char c = getCharacter();
        if (show)
        {
            if ((mask == (char) 255) || (c == '\n'))
                printCharacter(c);
            else
                printCharacter(mask);
        }

        if (c == '\n')
            break;
        else
            buffer[length++] = c;
    }
    buffer[length] = '\0';
}

void rebootSystem(){ asm("int $0x19;"); }

void main(void)
{
    char input[BUFFER_SIZE];
    getLine(input, BUFFER_SIZE, TRUE, (char) 255);
    printLine(input);
}
