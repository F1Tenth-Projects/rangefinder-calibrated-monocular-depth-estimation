#include <samd51.h>
#include <panic.h>

void panic(void)
{
	PORT->Group[2].DIRSET.reg = PORT_PC30;
	PORT->Group[2].DIRSET.reg = PORT_PC31;
	while (1) {
		volatile long i;
		for (i = 0; i < 500000; i++);
		PORT->Group[2].OUTTGL.reg = PORT_PC30;
		PORT->Group[2].OUTTGL.reg = PORT_PC31;
	}
}
