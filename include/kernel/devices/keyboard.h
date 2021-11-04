/*
 * EOS - Experimental Operating System
 * Keyboard driver header
 */

#ifndef _KERNEL_KEYBOARD_H_
#define _KERNEL_KEYBOARD_H_

#include <stdint.h>
#include <stddef.h>

//#include <arch/i686/io.h>
//#include <arch/i686/devices.h>

//#include <stdio.h>
//#include <tty_old.h>
//#include <string.h>

#define KBD_DATA 0x60

#define KEY_ESC         0x1
#define KEY_BACKSPACE   0xE
#define KEY_TAB         0xF

#define KEY_P           0x19
#define KEY_ENTER       0x1C
#define KEY_S           0x1F
#define KEY_LCTRL       0x1D
#define KEY_F           0x21
#define KEY_LSHIFT      0x2A
#define KEY_C           0x2E

#define KEY_RSHIFT      0x36
#define KEY_RCTRL       0x38

#define KEY_HOME        0x147
#define KEY_ARROW_UP    0x148
#define KEY_ARROW_LEFT  0x14B
#define KEY_ARROW_RIGHT 0x14D
#define KEY_END         0x14F
#define KEY_ARROW_DOWN  0x150
#define KEY_DELETE      0x153

typedef struct {
    int exists  : 1;
    int release : 1;

    int lshift  : 1;
    int rshift  : 1;

    int lctrl   : 1;
    int rctrl   : 1;

    int numlk   : 1;
    int capslk  : 1;
    int scrllk  : 1;

    int code    : 11;
} __attribute__((packed)) kbd_event;

void keyboard_install();

uint8_t keyboard_getchar();
size_t keyboard_gets(char* s, size_t);
//size_t kbd_getsn(char*, size_t);

void keyboard_waitForBufToEmpty();
void keyboard_reset_cpu();

//void keyboard_scancodes_setup();

void keyboard_wait_irq();
kbd_event keyboard_buffer_pop();
char keyboard_event_convert(kbd_event e);

#endif
