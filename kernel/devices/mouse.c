/*
 * EOS - Experimental Operating System
 * Mouse driver
 */

#include <kernel/devices/mouse.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/interrupts.h>
#include <kernel/io/ports.h>

#include <kernel/tty.h>

#include <kernel/libk/string.h>

uint8_t mouse_ready = 0;

int32_t mouse_x = 0;
int32_t mouse_y = 0;

uint8_t mouse_b1 = 0;
uint8_t mouse_b2 = 0;
uint8_t mouse_b3 = 0;
uint8_t mouse_b4 = 0;
uint8_t mouse_b5 = 0;

int mouse_wheel = 0; // change to 0 after each reading

// cursor image in format 0xAARRGGBB
uint32_t cursor[32][32] = {
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X84000000, 0X06000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XBC000000, 0X0C000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0XCA000000, 0X14000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0XFF000000, 0XD7000000, 0X1D000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0XB8000000, 0XFF000000, 0XE2000000, 0X28000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X0C000000, 0XA0000000, 0XFF000000, 0XEB000000, 0X35000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0X01000000, 0X8D000000, 0XFF000000, 0XF3000000, 0X43000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X79000000, 0XFF000000, 0XF9000000, 0X54000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X65000000, 0XFD000000, 0XFD000000, 0X66000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X53000000, 0XF8000000, 0XFF000000, 0X79000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X43000000, 0XF3000000, 0XFF000000, 0X8E000000, 0X01000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X34000000, 0XEB000000, 0XFF000000, 0XA1000000, 0X04000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X27000000, 0XE2000000, 0XFF000000, 0XB2000000, 0X08000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X1C000000, 0XD6000000, 0XFF000000, 0XC2000000, 0X0F000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X13000000, 0XCA000000, 0XFF000000, 0XCF000000, 0X17000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X0C000000, 0XBB000000, 0XFF000000, 0XDC000000, 0X21000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X06000000, 0XAA000000, 0XFF000000, 0XE6000000, 0X2D000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X02000000, 0X98000000, 0XFF000000, 0XEE000000, 0X3A000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X30000000, 0X40000000, 0X51000000, 0X61000000, 0X71000000, 0X81000000, 0X92000000, 0XF1000000, 0XFF000000, 0XF5000000, 0X49000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XBE000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFF000000, 0XFA000000, 0X58000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0X3C000000, 0X8D000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X4D000000, 0XFF000000, 0XED000000, 0X7C000000, 0X6C000000, 0X5C000000, 0X4C000000, 0X3C000000, 0X2C000000, 0X1D000000, 0X0D000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X43000000, 0XF4000000, 0XF7000000, 0X14000000, 0XFFFFFFFF, 0X02000000, 0XDB000000, 0XFF000000, 0X2D000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0XFFFFFFFF, 0X4B000000, 0XF7000000, 0XFF000000, 0XFF000000, 0X7B000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X6C000000, 0XFF000000, 0X9D000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X08000000, 0X54000000, 0XFA000000, 0XFD000000, 0X86000000, 0XFE000000, 0XE6000000, 0X05000000, 0XFFFFFFFF, 0X0C000000, 0XF0000000, 0XF8000000, 0X16000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0X65000000, 0XFC000000, 0XFC000000, 0X5C000000, 0X00FFFFFF, 0XB4000000, 0XFF000000, 0X5B000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X8B000000, 0XFF000000, 0X7E000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0XFE000000, 0XFA000000, 0X53000000, 0X00FFFFFF, 0X00FFFFFF, 0X43000000, 0XFF000000, 0XCC000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X1F000000, 0XFC000000, 0XE8000000, 0X06000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XFF000000, 0XF7000000, 0X4A000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD2000000, 0XFF000000, 0X3C000000, 0XFFFFFFFF, 0XFFFFFFFF, 0XAA000000, 0XFF000000, 0X5F000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XD8000000, 0XF4000000, 0X42000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X62000000, 0XFF000000, 0XAC000000, 0XFFFFFFFF, 0XFFFFFFFF, 0X3A000000, 0XFF000000, 0XD0000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0XC8000000, 0X3A000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X07000000, 0XEA000000, 0XFC000000, 0X1F000000, 0X03000000, 0X55000000, 0XF7000000, 0XFF000000, 0X41000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X1D000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X81000000, 0XFF000000, 0XBF000000, 0XDF000000, 0XFF000000, 0XFF000000, 0XD5000000, 0X46000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X17000000, 0XF9000000, 0XFF000000, 0XFE000000, 0XB8000000, 0X47000000, 0X01000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF },
    { 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X96000000, 0X99000000, 0X2A000000, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF, 0X00FFFFFF }
};

uint32_t under_cursor[32][32];

// This shouldn't be used outside of mouse driver.
typedef struct mouse_flags_byte {
    unsigned int left_button : 1;
    unsigned int right_button : 1;
    unsigned int middle_button : 1;

    unsigned int always1 : 1;

    unsigned int x_sign : 1;
    unsigned int y_sign : 1;

    unsigned int x_overflow : 1;
    unsigned int y_overflow : 1;
} __attribute__((packed)) mouse_flags_byte;

/*extern*/ struct dev_ps2m_mouse_packet {
    int16_t movement_x;
    int16_t movement_y;
    uint8_t button_l;
    uint8_t button_m;
    uint8_t button_r;
} ps2m_buffer;

void mouse_parse_packet(const char *buf, uint8_t has_wheel, uint8_t has_5_buttons)
{
    mouse_flags_byte *mfb = (mouse_flags_byte *)(buf);
    if (mfb->x_overflow || mfb->y_overflow || !mfb->always1) {
        return;
    }

    int offx = (int16_t)(0xff00 * mfb->x_sign) | buf[1];
    int offy = (int16_t)(0xff00 * mfb->y_sign) | buf[2];
    mouse_x += offx;
    mouse_y -= offy;
    mouse_b1 = mfb->left_button;
    mouse_b2 = mfb->right_button;
    mouse_b3 = mfb->middle_button;
    ps2m_buffer.movement_x = offx;
    ps2m_buffer.movement_y = offy;
    ps2m_buffer.button_l = mouse_b1;
    ps2m_buffer.button_r = mouse_b2;
    ps2m_buffer.button_m = mouse_b3;

    if (has_wheel) {
        // Parse wheel (byte 3, bits 0-3)
        mouse_wheel += (char)((!!(buf[3] & 0x8)) * 0xf8 | (buf[3] & 0x7));
        if (has_5_buttons) {
            mouse_b4 = !!(buf[3] & 0x10);
            mouse_b4 = !!(buf[3] & 0x20);
            // parse buttons 4-5 (byte 3, bits 4-5)
        }
    }
}

void mouse_erase()
{
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            set_pixel(mouse_x + j - 1, mouse_y + i - 1, under_cursor[i][j]);
        }
    }
}

void mouse_draw()
{
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            under_cursor[i][j] = get_pixel(mouse_x + j - 1, mouse_y + i - 1);
        }
    }
    rgba_color rc;
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            rc.b = cursor[i][j] & 0xFF;
            rc.g = (cursor[i][j] >> 8) & 0xFF;
            rc.r = (cursor[i][j] >> 16) & 0xFF;
            rc.a = (cursor[i][j] >> 24) & 0xFF;

            // set_pixel(mouse_x + j - 1, mouse_y + i - 1, cursor[i][j]);
            set_pixel_alpha(mouse_x + j - 1, mouse_y + i - 1, rc);
        }
    }
}

void mouse_handler(__attribute__((unused)) struct regs *r)
{ // (not used but just there)
    //tty_printf("mouse!\n");
    //tty_printf("mouse_x = %d  mouse_y = %d\n", mouse_x, mouse_y);

    /*
    static uint8_t punymutex = 0;
    if (punymutex) {
        return;
    }
    punymutex = 1;
    */

    uint8_t status = inb(0x64);
    if ((status & 1) == 0 || (status >> 5 & 1) == 0) {
        return;
    }

    static int recbyte = 0;
    static char mousebuf[5];

    mousebuf[recbyte++] = inb(0x60);
    if (recbyte == 3 /* + has_wheel */) {
        recbyte = 0;

        static uint8_t drawn = 0;
        if (drawn) {
            mouse_erase();
        }
        drawn = 1;

        mouse_parse_packet(mousebuf, 0, 0);

        // Bounds
        if (mouse_x < 0) {
            //tty_printf("x < 0 \n");
            mouse_x = 0;
        }
        if (mouse_y < 0) {
            //tty_printf("y < 0 \n");
            mouse_y = 0;
        }
        if (mouse_x > (int)(VESA_WIDTH)) {
            //tty_printf("x > W \n");
            mouse_x = VESA_WIDTH; //-10;
        }
        if (mouse_y > (int)(VESA_HEIGHT)) {
            //tty_printf("y > H \n");
            mouse_y = VESA_HEIGHT; //-10;
        }

        // Redraw the cursor
        mouse_draw();
    }

    //punymutex = 0;
}

void mouse_wait(uint8_t a_type)
{                                //unsigned char
    uint32_t _time_out = 100000; //unsigned int
    if (a_type == 0) {
        while (_time_out--) { //Data
            if ((inb(0x64) & 1) == 1) {
                return;
            }
        }
        return;
    } else {
        while (_time_out--) { //Signal
            if ((inb(0x64) & 2) == 0) {
                return;
            }
        }
        return;
    }
}

void mouse_write(uint8_t a_write)
{ //unsigned char
    //Wait to be able to send a command
    mouse_wait(1);
    //Tell the mouse we are sending a command
    outb(0x64, 0xD4);
    //Wait for the final part
    mouse_wait(1);
    //Finally write
    outb(0x60, a_write);
}

uint8_t mouse_read()
{
    //Gets response from mouse
    mouse_wait(0);
    return inb(0x60);
}

void mouse_install()
{
    memset(under_cursor, 0, 32 * 32 * sizeof(uint32_t));

    /*
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 5; j++) {
            pixels[i][j] = getPix(mx + j, mouse_y + i);
        }
    }
    */

    uint8_t _status; //unsigned char

    // Enable the auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable the interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    _status = (inb(0x60) | 2);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, _status);

    // Tell the mouse to use default settings
    mouse_write(0xF6);
    mouse_read(); // Acknowledge

    // Enable the mouse
    mouse_write(0xF4);
    mouse_read(); // Acknowledge

    // Set cursor coordinates in middle of the screen
    mouse_x = VESA_WIDTH / 2;
    mouse_y = VESA_HEIGHT / 2;

    //Setup mouse IRQ handler. We are doing it before mouse initalization, so first interrupt will be catched anyway
    //tty_printf("hello!!!!!!!");
    register_interrupt_handler(MOUSE_IDT_INDEX, &mouse_handler);
    mouse_ready = 1;
}
