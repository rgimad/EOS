/*
 * EOS - Experimental Operating System
 * Port io functions
 */

#include <kernel/io/ports.h>

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

inline void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
}

inline void outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %0, %1" ::"a"(val), "Nd"(port));
}

inline void outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %1" ::"a"(val), "Nd"(port));
}

inline uint64_t rdtsc()
{
    uint64_t ret;
    asm volatile("rdtsc"
                 : "=A"(ret));
    return ret;
}

inline void insl(unsigned short port, unsigned int buffer, unsigned long count)
{
    asm("cld; rep; insl" ::"D"(buffer), "d"(port), "c"(count));
}

int com1_is_transmit_empty()
{
    return inb(PORT_COM1 + 5) & 0x20;
}

void com1_write_char(char a)
{
    while (com1_is_transmit_empty() == 0)
        ;
    outb(PORT_COM1, a);
}
