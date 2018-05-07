#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

#define NULL 0

/* needs to stay the first line */
asm("jmp $0, $main");

/* space for additional code */
void printCharacter(char toPrint)
{
    asm(
        "int $0x10;"
        :
        : "a" (0x0E00 | toPrint)
    );
    return;
}

void printString(char* toPrint)
{
    for (int i = 0; toPrint[i] != '\0'; i++)
    {
        printCharacter(toPrint[i]);
    }
}

char getCharacter(int bShow, char mask)
{
    char c;
    asm(
        "int $0x10;"
        :"=a"(0x0800 | c)
    );
    if (bShow != 0)
    {
        if ((mask == (char) 255) || (c == '\n')){
            printCharacter(c);
        } else {
            printCharacter(mask);
        }
    }
    return c;
}

char* getString(char* retVal, int bufferLength, int bShow, char mask)
{
    int length = 0;
    while (length < bufferLength)
    {
        char newC = getCharacter(bShow, mask);
        if (newC == '\n')
        {
            break;
        } else {
            retVal[length++] = newC;
        }
    }
    retVal[length] = '\0';
    return retVal;
}

void rebootSystem(){ asm("int $0x19;"); }

void main(void)
{
	asm("jmp .");
}
