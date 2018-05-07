#ifndef _CODE16GCC_H_
#define _CODE16GCC_H_
asm(".code16gcc\n");
#endif

/* needs to stay the first line */
asm("jmp $0, $main");

/* space for additional code */

void main(void)
{
	asm("jmp .");
}
