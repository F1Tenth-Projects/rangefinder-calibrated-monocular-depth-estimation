#include <samd51.h>
#include <gpio.h>

void gpio_set_direction(int bank, int pin, int dir_out)
{
	if (dir_out) {
		PORT->Group[bank].DIRSET.reg = 1 << pin;
	} else {
		PORT->Group[bank].DIRCLR.reg = 1 << pin;
	}
}

void gpio_set_value(int bank, int pin, int value)
{
	if (value) {
		PORT->Group[bank].OUTSET.reg = 1 << pin;
	} else {
		PORT->Group[bank].OUTCLR.reg = 1 << pin;
	}
}
