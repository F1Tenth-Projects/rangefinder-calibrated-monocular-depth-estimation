#ifndef GPIO_H
#define GPIO_H

void gpio_set_direction(int bank, int pin, int dir_out);
void gpio_set_value(int bank, int pin, int value);

#endif
