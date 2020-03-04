/*
*    EOS - Experimental Operating System
*    Programmable interval timer (PIT)
*/
#include <kernel/devices/timer.h>
#include <kernel/idt.h>
#include <kernel/interrupts.h>
#include <kernel/io/qemu_log.h>

#include <kernel/pm/scheduler.h>

// Holds how many ticks that the system has been running for
int timer_ticks = 0;

void timer_set_frequency(int hz)
{
	int divisor = INPUT_CLOCK_FREQUENCY / hz;
	outb(TIMER_COMMAND, TIMER_OCW); // means Init, Square Wave Mode, non-BCD, first transfer LSB then MSB
	outb(TIMER_DATA, divisor & 0xFF); // Set low byte of divisor
 	outb(TIMER_DATA, divisor >> 8); // Set high byte of divisor
 	//outb(TIMER_DATA, (divisor >> 8) & 0xFF);
}

// IRQ Handler for the timer. Called at every clock tick
void timer_handler(struct regs *r)
{
	timer_ticks++;
	if (timer_ticks % 5000 == 0)
	{
		qemu_printf("ticks = %d\n", timer_ticks);
	}
	schedule(r);
}

int timer_get_ticks() { return timer_ticks; }

// Timer init
void timer_install()
{
	register_interrupt_handler(TIMER_IDT_INDEX, timer_handler);
	timer_set_frequency(TICKS_PER_SECOND);
}