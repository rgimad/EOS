/*
*    EOS - Experimental Operating System
*    Programmable interval timer (PIT) header
*/
#ifndef _TIMER_H_
#define _TIMER_H_

#define INPUT_CLOCK_FREQUENCY 1193180
#define TIMER_COMMAND 0x43
#define TIMER_DATA 0x40
#define TIMER_OCW 0x36 // Operational command word which means Init, Square Wave Mode, non-BCD, first transfer LSB then MSB

#define TICKS_PER_SECOND 100


void timer_install();
int timer_get_ticks();

#endif  // _TIMER_H_