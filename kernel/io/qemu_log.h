/*
 * EOS - Experimental Operating System
 * Qemu logging functions header
 */

#ifndef _QEMU_LOG_H
#define _QEMU_LOG_H

#include <kernel/libk/sprintf.h>
#include <kernel/io/ports.h>

void qemu_printf(const char *format, ...);

#endif
