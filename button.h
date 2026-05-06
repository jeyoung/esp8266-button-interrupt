#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "osapi.h"

#define PIN_BUTTON 2
#define PIN_LED 0
#define DEBOUNCE_MS 30

extern volatile uint32 elapsed;

void button_init(void);
void button_pressed(int pin);

#endif
