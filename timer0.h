#ifndef TIMER0_H_
#define TIMER0_H_

#include <avr/io.h>

/* Global system uptime in milliseconds */
extern volatile unsigned long system_time_ms;  

void timer0_init(void);

#endif