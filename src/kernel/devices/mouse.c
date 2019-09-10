/*
*    EOS - Experimental Operating System
*    Mouse driver
*/
#include <kernel/devices/mouse.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/interrupts.h>
#include <kernel/io/ports.h>

#include <kernel/tty.h>

#include <libk/string.h>


uint8_t mouse_ready = 0;
int32_t mouse_x  = 0;
int32_t mouse_y  = 0;
uint8_t mouse_b1 = 0;
uint8_t mouse_b2 = 0;
uint8_t mouse_b3 = 0;
uint8_t mouse_b4 = 0;
uint8_t mouse_b5 = 0;
int     mouse_wheel = 0; // change to 0 after each reading


uint32_t under_cursor[32][32];

//This shouldn't be used outside of mouse driver.
typedef struct mouse_flags_byte
{
	unsigned int   left_button : 1;
	unsigned int  right_button : 1;
	unsigned int middle_button : 1;
	unsigned int       always1 : 1;
	unsigned int        x_sign : 1;
	unsigned int        y_sign : 1;
	unsigned int    x_overflow : 1;
	unsigned int    y_overflow : 1;
} __attribute__((packed)) mouse_flags_byte;

/*extern */struct dev_ps2m_mouse_packet {
    int16_t movement_x;
    int16_t movement_y;
    uint8_t button_l;
    uint8_t button_m;
    uint8_t button_r;
} ps2m_buffer;

void mouse_parse_packet(const char* buf, uint8_t has_wheel, uint8_t has_5_buttons)
{
	mouse_flags_byte* mfb = (mouse_flags_byte*)(buf);
	if(mfb->x_overflow || mfb->y_overflow || !mfb->always1)
		return;
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


	if(has_wheel)
	{
		// parse wheel (byte 3, bits 0-3)
		mouse_wheel += (char)((!!(buf[3] & 0x8)) * 0xf8 | (buf[3] & 0x7));
		if(has_5_buttons)
		{
			mouse_b4 = !!(buf[3] & 0x10);
			mouse_b4 = !!(buf[3] & 0x20);
			// parse buttons 4-5 (byte 3, bits 4-5)
		}
	}
}

void mouse_erase()
{
	for(int i = 0; i < 32; ++i)
	{
		for(int j = 0; j < 32; ++j)
		{
			set_pixel(mouse_x + j - 1, mouse_y + i - 1, under_cursor[i][j]);
		}
	}
}

void mouse_draw()
{

	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			under_cursor[i][j] = get_pixel(mouse_x + j - 1, mouse_y + i - 1);
		}
	}
	for(int y = 0; y < 20; y++)
	{
		for(int x = 0; x < y + 1 && x + y * 2 < 40; x++)
		{
			set_pixel(mouse_x + x, mouse_y + y, VESA_WHITE);
		}
	}
}

void mouse_handler(__attribute__((unused)) struct regs *r) // (not used but just there)
{
	//tty_printf("mouse!\n");
	//tty_printf("mouse_x = %d  mouse_y = %d\n", mouse_x, mouse_y);

	/*static uint8_t punymutex = 0;
	if(punymutex)
		return;
	punymutex = 1;*/

	uint8_t status=inb(0x64);
	if((status & 1) == 0 || (status >> 5 & 1) == 0)
		return;

	static int recbyte = 0;
	static char mousebuf[5];

	mousebuf[recbyte++] = inb(0x60);
	if(recbyte == 3 /* + has_wheel */)
	{

		recbyte = 0;

		static uint8_t drawn = 0;
		if(drawn)
		{
			mouse_erase();
		}
		drawn = 1;

		mouse_parse_packet(mousebuf, 0, 0);

		// Bounds
		if(mouse_x < 0)
		{
			//tty_printf("x < 0 \n");
			mouse_x = 0;
		}
		if(mouse_y < 0)
		{
			//tty_printf("y < 0 \n");
			mouse_y = 0;
		}
		if(mouse_x > (int)(VESA_WIDTH))
		{
			//tty_printf("x > W \n");
			mouse_x = VESA_WIDTH;//-10;
		}
		if(mouse_y > (int)(VESA_HEIGHT))
		{
			//tty_printf("y > H \n");
            mouse_y = VESA_HEIGHT;//-10;
		}

		// Redraw the cursor
		mouse_draw();
	}

	//punymutex = 0;
}

void mouse_wait(uint8_t a_type) //unsigned char
{
	uint32_t _time_out=100000; //unsigned int
	if(a_type==0)
	{
		while(_time_out--) //Data
		{
			if((inb(0x64) & 1)==1)
			{
				return;
			}
		}
		return;
	}
	else
	{
		while(_time_out--) //Signal
		{
			if((inb(0x64) & 2)==0)
			{
				return;
			}
		}
		return;
	}
}

void mouse_write(uint8_t a_write) //unsigned char
{
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
	memset(under_cursor, 0, 32*32*sizeof(uint32_t));
	/*for(int i=0; i<8; i++)
		for(int j=0; j<5; j++)
			pixels[i][j]=getPix(mx+j,mouse_y+i);*/
	uint8_t _status;  //unsigned char
	//Enable the auxiliary mouse device
	mouse_wait(1);
	outb(0x64, 0xA8);
	//Enable the interrupts
	mouse_wait(1);
	outb(0x64, 0x20);
	mouse_wait(0);
	_status=(inb(0x60) | 2);
	mouse_wait(1);
	outb(0x64, 0x60);
	mouse_wait(1);
	outb(0x60, _status);
	//Tell the mouse to use default settings
	mouse_write(0xF6);
	mouse_read();  //Acknowledge
	//Enable the mouse
	mouse_write(0xF4);
	mouse_read();  //Acknowledge

	//Set cursor coordinates in middle of the screen.
	mouse_x = VESA_WIDTH / 2;
	mouse_y = VESA_HEIGHT / 2;
	//Setup mouse IRQ handler. We are doing it before mouse initalization, so first interrupt will be catched anyway.
	//tty_printf("hello!!!!!!!");
	register_interrupt_handler(MOUSE_IDT_INDEX, &mouse_handler);
	mouse_ready = 1;
}