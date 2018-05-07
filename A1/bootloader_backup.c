#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

// asm("mov $0x17E0, %esp;");
// asm("mov $0x17E0, %ebp;");

#define BUFFER_LENGTH 3
// char input[BUFFER_LENGTH];
// int length = 0;
// char c;

/* needs to stay the first line */
asm("jmp $0, $main;");

// #define NULL 0

// typedef int bool;
// #define TRUE 1
// #define FALSE 0

/* space for additional code */
// void printCharacter(char toPrint)
// {
//     asm volatile(
//         "int $0x10;"
//         :: "a" (0x0E00 | toPrint)
//     );
// }

// void printLine(char * toPrint)
// {
//     while (*toPrint)
//         printCharacter(*(toPrint++));
    
//     printCharacter('\n');
//     printCharacter('\r');
// }

// char getCharacter(void)
// {
//     char c;
//     asm volatile(
//         "int $0x16;"
//         :"=al"(c)
//         :"ah"(0x00)
//     );
//     return c;
// }

// void getLine(char * buffer, int bufferLength)
// {
//     int length = 0;
//     while (length < bufferLength)
//     {
//         char c = getCharacter();
//         if (c == '\n')
//             break;
//         else
//         {
//             printCharacter('*');
//             buffer[length++] = c;
//         }   
//     }
//     buffer[length] = '\0';
// }

// void rebootSystem(){ asm("int $0x19;"); }

void main(void)
{
    // char buffer[] = {'1', '2', '3', '4', '5'};
    // short index = 0;
    // while (length < BUFFER_LENGTH - 1)
    while( 1 )
    {
        char c;
        asm volatile(
            "int $0x16;"
            :"=al"(c)
            :"ah"(0x00)
        );
        // buffer[(index++)%5] = c;
        asm volatile(
            "int $0x10;"
            :: "a" (0x0E00 | c)
        );
        if (c == '\r') {
        asm volatile(
            "int $0x10;"
            "mov $0x0E, %%ah;"
            "mov $0x0D, %%al;"
            "int $0x10;"
            :
            : "ax"(0x0E0A)
            : // "ax"
        );    
        //     for (short i = 0; i < 5; i++){
        //         asm volatile(
        //             "int $0x10;"
        //             :: "a" (0x0E00 | buffer[i])
        //         );    
        //     }
             // break;
        // else
        // {
            
            // printCharacter('n');
            // printCharacter(c);
            // input[length] = c;
            // length++;
        }
    }
    // input[length] = '\0';
    
    // printLine("");
    // printLine(input);

    // while (1);
}
