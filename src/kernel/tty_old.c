/*
*    EOS - Experimental Operating System
*    Terminal subsystem
*/
#include <kernel/tty.h>
#include <kernel/vga.h>

#include <libk/string.h>

volatile uint8_t tty_feedback = 1;

size_t tty_line_fill[VGA_WIDTH];
size_t tty_row;
size_t tty_column;
uint8_t tty_color;
uint16_t* tty_buffer;

//string.h part (for future)
/*size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != 0) {
        len++;
    }
    return len;
}

int strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
        if (*s1 == '\0')
            return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}*/




void update_cursor(size_t row, size_t col) {
  unsigned short position = (row * VGA_WIDTH) + col;

  // cursor LOW port to vga INDEX register
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(position & 0xFF));
  // cursor HIGH port to vga INDEX register
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

void tty_init(void) {
  tty_row = 0;
  tty_column = 0;
  tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  tty_buffer = VGA_MEMORY;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      tty_buffer[index] = vga_entry(' ', tty_color);
    }
  }
}

void tty_backspace() {
  if (tty_column == 0) {
    if (tty_row > 0) {
      tty_row--;
    }
    tty_column = tty_line_fill[tty_row];
  } else {
    tty_column--;
  }

  tty_putentryat(32, tty_color, tty_column, tty_row);
  update_cursor(tty_row, tty_column);
}

void tty_setcolor(uint8_t color) { tty_color = color; }

void tty_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  tty_buffer[index] = vga_entry(c, color);
}

void tty_putchar(char c) {
  if (c != '\n') {
    tty_putentryat(c, tty_color, tty_column, tty_row);
  }

  if (++tty_column == VGA_WIDTH || c == '\n') {
    tty_line_fill[tty_row] = tty_column - 1;
    tty_column = 0;
    if (++tty_row == VGA_HEIGHT) {
      tty_scroll();
    }
  }

  update_cursor(tty_row, tty_column);
}

void tty_scroll() {
  tty_row--;
  for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t src_index = y * VGA_WIDTH + x;
      const size_t dstty_index = (y + 1) * VGA_WIDTH + x;
      tty_buffer[src_index] = tty_buffer[dstty_index];
    }
    tty_line_fill[y] = tty_line_fill[y + 1];
  }

  for (size_t x = 0; x < VGA_WIDTH; x++) {
    const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
    tty_buffer[index] = vga_entry(' ', tty_color);
  }
}

void tty_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) tty_putchar(data[i]);
}

void tty_putstring(const char* data) { tty_write(data, strlen(data)); }

void tty_putuint(int i)
{
    unsigned int n, d = 1000000000;
    char str[255];
    unsigned int dec_index = 0;
    while( ( i/d == 0 ) && ( d >= 10 ) ) d /= 10;
    n = i;
    while(d >= 10)
    {
        str[dec_index++] = ((char)((int)'0' + n/d));
        n = n % d;
        d /= 10;
    }
    str[dec_index++] = ((char)((int)'0' + n));
    str[dec_index] = 0;
    tty_putstring(str);
}

void tty_putint(int i)
{
    if(i >= 0)
    {
        tty_putuint(i);
    } else {
        tty_putchar('-');
        tty_putuint(-i);
    }
}


void tty_puthex(uint32_t i)
{
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;

    tty_putstring("0x");
    while((i / d == 0) && (d >= 0x10)) d /= 0x10;
    n = i;
    while( d >= 0xF )
    {
        tty_putchar(hex[n/d]);
        n = n % d;
        d /= 0x10;
    }
    tty_putchar(hex[n]);
}


void tty_print(char *format, va_list args)
{
    int i = 0;
    char *string;

    while (format[i])
    {
        if (format[i] == '%')
        {
            i++;
            switch (format[i])
            {
                case 's':
                    string = va_arg(args, char*);
                    tty_putstring(string);
                    break;
                case 'c':
                    // To-Do: fix this! "warning: cast to pointer from integer of different size"
                    tty_putchar(va_arg(args, int));
                    break;
                case 'd':
                    tty_putint(va_arg(args, int));
                    break;
                case 'i':
                    tty_putint(va_arg(args, int));
                    break;
                case 'u':
                    tty_putuint(va_arg(args, unsigned int));
                    break;
                case 'x':
                    tty_puthex(va_arg(args, uint32_t));
                    break;
                default:
                    tty_putchar(format[i]);
            }
        } else {
            tty_putchar(format[i]);
        }
        i++;
    }//endwhile
}

void tty_printf(char *text, ... )
{
    va_list args;
    // find the first argument
    va_start(args, text);
    // pass print the output handle the format text and the first argument
    tty_print(text, args);
}

void tty_putstring_color(const char* data, uint8_t text_color)
{
    uint8_t tty_color_old = tty_color;
    tty_setcolor(vga_entry_color(text_color, VGA_COLOR_BLACK));
    tty_putstring(data);
    tty_setcolor(tty_color_old);
}