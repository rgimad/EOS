/*
 * EOS - Experimental Operating System
 * Port io functions header
 */

#ifndef _PORTS_H
#define _PORTS_H

#include <stdint.h> // to use uint8_t and etc

#define PORT_COM1 0x3f8

uint8_t inb(uint16_t port); // Read a byte from port
uint16_t inw(uint16_t port); // Read a word from port
uint32_t inl(uint16_t port); // Read a dword from port

void outb(uint16_t port, uint8_t  val); // Write a byte into port
void outw(uint16_t port, uint16_t val); // Write a word into port
void outl(uint16_t port, uint32_t val); // Write a dword into port

uint64_t rdtsc(); // Get the the number of cycles since cpu reset

void insl(unsigned short port, unsigned int buffer, unsigned long count); // Read <count> unsignedint's(=dword?) from <port> to <buffer>

int com1_is_transmit_empty();
void com1_write_char(char a);

#endif
