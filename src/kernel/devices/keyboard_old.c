/*
*    EOS - Experimental Operating System
*    PS/2 Keyboard support
*/
#include <kernel/devices/keyboard.h>
#include <kernel/idt.h>
#include <kernel/interrupts.h>
#include <kernel/tty.h>
//#include <stdio.h>

struct kb_state {
  int caps_lock;
  int shift_held;
};

struct kb_state state;

// Scancode table used to layout a standard US keyboard.
// Uses the second row if SHIFT is held
// Uses the third row if CAPS LOCK is held
// Uses the fourth row if both SHIFT and CAPS LOCK are held
unsigned char kbdus[128][4] = {
    {0, 0, 0, 0},
    {27, 27, 27, 27},
    {'1', '!', '1', '!'},
    {'2', '@', '2', '@'},
    {'3', '#', '3', '#'},
    {'4', '$', '4', '$'},
    {'5', '%', '5', '%'},
    {'6', '^', '6', '^'},
    {'7', '&', '7', '&'},
    {'8', '*', '8', '*'},  // index 9
    {'9', '(', '9', '('},
    {'0', ')', '0', ')'},
    {'-', '_', '-', '_'},
    {'=', '+', '=', '+'},
    {'\b', '\b', '\b', '\b'},  // Backspace
    {'\t', '\t', '\t', '\t'},  // Tab
    {'q', 'Q', 'Q', 'q'},
    {'w', 'W', 'W', 'w'},
    {'e', 'E', 'E', 'e'},
    {'r', 'R', 'R', 'r'},  // index 19
    {'t', 'T', 'T', 't'},
    {'y', 'Y', 'Y', 'y'},
    {'u', 'U', 'U', 'u'},
    {'i', 'I', 'I', 'i'},
    {'o', 'O', 'O', 'o'},
    {'p', 'P', 'P', 'p'},
    {'[', '{', '[', '{'},
    {']', '}', ']', '}'},
    {'\n', '\n', '\n', '\n'},  // Enter key
    {0, 0, 0, 0},              // Control key, index 29
    {'a', 'A', 'A', 'a'},
    {'s', 'S', 'S', 's'},
    {'d', 'D', 'D', 'd'},
    {'f', 'F', 'F', 'f'},
    {'g', 'G', 'G', 'g'},
    {'h', 'H', 'H', 'h'},
    {'j', 'J', 'J', 'j'},
    {'k', 'K', 'K', 'k'},
    {'l', 'L', 'L', 'l'},
    {';', ':', ';', ':'},  // index 39
    {'\'', '\"', '\'', '\"'},
    {'`', '~', '`', '~'},
    {0, 0, 0, 0},  // Left Shift
    {'\\', '|', '\\', '|'},
    {'z', 'Z', 'Z', 'z'},
    {'x', 'X', 'X', 'x'},
    {'c', 'C', 'C', 'c'},
    {'v', 'V', 'V', 'v'},
    {'b', 'B', 'B', 'b'},
    {'n', 'N', 'N', 'n'},  // index 49
    {'m', 'M', 'M', 'm'},
    {',', '<', ',', '<'},
    {'.', '>', '.', '>'},
    {'/', '?', '/', '?'},
    {0, 0, 0, 0},  // Right Shift
    {'*', '*', '*', '*'},
    {0, 0, 0, 0},          // Alt
    {' ', ' ', ' ', ' '},  // Space
    {0, 0, 0, 0},          // Caps Lock
    {0, 0, 0, 0},          // F1 key, index 59
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},  // F10
    {0, 0, 0, 0},  // Num Lock, index 69
    {0, 0, 0, 0},  // Scroll Lock
    {0, 0, 0, 0},  // Home Key
    {0, 0, 0, 0},  // Up Arrow
    {0, 0, 0, 0},  // Page Up
    {'-', '-', '-', '-'},
    {0, 0, 0, 0},  // Left Arrow
    {0, 0, 0, 0},
    {0, 0, 0, 0},  // Right Arrow
    {'+', '+', '+', '+'},
    {0, 0, 0, 0},  // End Key, index 79
    {0, 0, 0, 0},  // Down Arrow
    {0, 0, 0, 0},  // Page Down
    {0, 0, 0, 0},  // Insert Key
    {0, 0, 0, 0},  // Delete Key
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},  // F11 Key
    {0, 0, 0, 0},  // F12 Key
    {0, 0, 0, 0},  // All other keys are undefined
};

// Handles the keyboard interrupt
void keyboard_handler(__attribute__((unused)) struct regs *r) {
  unsigned char scancode;
  int column = 0;
  char clicked = 0;
  // Read from the keyboard's data buffer
  scancode = inb(0x60);
  // If the top bit of the scancode is set, a key has just been released
  if (scancode & 0x80) {
    if (scancode >> 2 == 42 || scancode >> 2 == 54) {
      state.shift_held = 0;
    }
  } else {
    switch (scancode) {
      case 14:  // backspace
        tty_printf("<backspace>");
        //t_backspace();
        break;
      case 42:  // shifts
      case 54:
        state.shift_held = 1;
        break;
      case 58:  // caps lock
        state.caps_lock = !state.caps_lock;
        break;
      default:
        column = state.shift_held * 1 + state.caps_lock * 2;
        clicked = kbdus[scancode][column];
        if (clicked != 0 && clicked != 27) {
          tty_putchar(clicked);
        }
        break;
    }
  }
}

// Sets up the keyboard
void keyboard_install() {
  register_interrupt_handler(KEYBOARD_IDT_INDEX, keyboard_handler);
  state.caps_lock = 0;
  state.shift_held = 0;
  //printf("Keyboard installed.\n");
}