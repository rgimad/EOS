/*
 * Copyright (c) 2006-2007 -  http://brynet.biz.tm - <brynet@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* You need to include a file with fairly(ish) compliant tty_printf prototype, Decimal and String support like %s and %d and this is truely all you need! */
#include <kernel/cpu-detect.h>
#include <kernel/tty.h>



/* Simply call this function detect_cpu(); */
int detect_cpu(void) { /* or main() if your trying to port this as an independant application */
	unsigned long ebx, unused;
	cpuid(0, unused, ebx, unused, unused);
	switch(ebx) {
		case 0x756e6547: /* Intel Magic Code */
		do_intel();
		break;
		case 0x68747541: /* AMD Magic Code */
		do_amd();
		break;
		default:
		tty_printf("Unknown x86 CPU Detected\n");
		break;
	}
	return 0;
}

/* Intel Specific brand list */
char *Intel[] = {
	"Brand ID Not Supported.", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Intel(R) Pentium(R) III Xeon(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) III processor-M", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Xeon(R) Processor", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) 4 processor-M", 
	"Mobile Intel(R) Pentium(R) Celeron(R) processor", 
	"Reserved", 
	"Mobile Genuine Intel(R) processor", 
	"Intel(R) Celeron(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Celeron(R) processor", 
	"Mobile Geniune Intel(R) processor", 
	"Intel(R) Pentium(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor"
};

/* This table is for those brand strings that have two values depending on the processor signature. It should have the same number of entries as the above table. */
char *Intel_Other[] = {
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Celeron(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved"
};

/* Intel-specific information */
int do_intel(void) {
	tty_printf("Detected Intel CPU. Intel-specific features:\n");
	unsigned long eax, ebx, ecx, edx, max_eax, signature, unused;
	int model, family, type, brand, stepping, reserved;
	int extended_family = -1;
	cpuid(1, eax, ebx, unused, unused);
	model = (eax >> 4) & 0xf;
	family = (eax >> 8) & 0xf;
	type = (eax >> 12) & 0x3;
	brand = ebx & 0xff;
	stepping = eax & 0xf;
	reserved = eax >> 14;
	signature = eax;
	tty_printf("Type %d - ", type);
	switch(type) {
		case 0:
		tty_printf("Original OEM");
		break;
		case 1:
		tty_printf("Overdrive");
		break;
		case 2:
		tty_printf("Dual-capable");
		break;
		case 3:
		tty_printf("Reserved");
		break;
	}
	tty_printf("\n");
	tty_printf("Family %d - ", family);
	switch(family) {
		case 3:
		tty_printf("i386");
		break;
		case 4:
		tty_printf("i486");
		break;
		case 5:
		tty_printf("Pentium");
		break;
		case 6:
		tty_printf("Pentium Pro");
		break;
		case 15:
		tty_printf("Pentium 4");
	}
	tty_printf("\n");
	if(family == 15) {
		extended_family = (eax >> 20) & 0xff;
		tty_printf("Extended family %d\n", extended_family);
	}
	tty_printf("Model %d - ", model);
	switch(family) {
		case 3:
		break;
		case 4:
		switch(model) {
			case 0:
			case 1:
			tty_printf("DX");
			break;
			case 2:
			tty_printf("SX");
			break;
			case 3:
			tty_printf("487/DX2");
			break;
			case 4:
			tty_printf("SL");
			break;
			case 5:
			tty_printf("SX2");
			break;
			case 7:
			tty_printf("Write-back enhanced DX2");
			break;
			case 8:
			tty_printf("DX4");
			break;
		}
		break;
		case 5:
		switch(model) {
			case 1:
			tty_printf("60/66");
			break;
			case 2:
			tty_printf("75-200");
			break;
			case 3:
			tty_printf("for 486 system");
			break;
			case 4:
			tty_printf("MMX");
			break;
		}
		break;
		case 6:
		switch(model) {
			case 1:
			tty_printf("Pentium Pro");
			break;
			case 3:
			tty_printf("Pentium II Model 3");
			break;
			case 5:
			tty_printf("Pentium II Model 5/Xeon/Celeron");
			break;
			case 6:
			tty_printf("Celeron");
			break;
			case 7:
			tty_printf("Pentium III/Pentium III Xeon - external L2 cache");
			break;
			case 8:
			tty_printf("Pentium III/Pentium III Xeon - internal L2 cache");
			break;
		}
		break;
		case 15:
		break;
	}
	tty_printf("\n");
	cpuid(0x80000000, max_eax, unused, unused, unused);
	/* Quok said: If the max extended eax value is high enough to support the processor brand string
	(values 0x80000002 to 0x80000004), then we'll use that information to return the brand information. 
	Otherwise, we'll refer back to the brand tables above for backwards compatibility with older processors. 
	According to the Sept. 2006 Intel Arch Software Developer's Guide, if extended eax values are supported, 
	then all 3 values for the processor brand string are supported, but we'll test just to make sure and be safe. */
	if(max_eax >= 0x80000004) {
		tty_printf("Brand: ");
		if(max_eax >= 0x80000002) {
			cpuid(0x80000002, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		if(max_eax >= 0x80000003) {
			cpuid(0x80000003, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		if(max_eax >= 0x80000004) {
			cpuid(0x80000004, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		tty_printf("\n");
	} else if(brand > 0) {
		tty_printf("Brand %d - ", brand);
		if(brand < 0x18) {
			if(signature == 0x000006B1 || signature == 0x00000F13) {
				tty_printf("%s\n", Intel_Other[brand]);
			} else {
				tty_printf("%s\n", Intel[brand]);
			}
		} else {
			tty_printf("Reserved\n");
		}
	}
	tty_printf("Stepping: %d Reserved: %d\n", stepping, reserved);
	return 0;
}

/* Print Registers */
void printregs(int eax, int ebx, int ecx, int edx) {
	int j;
	char string[17];
	string[16] = '\0';
	for(j = 0; j < 4; j++) {
		string[j] = eax >> (8 * j);
		string[j + 4] = ebx >> (8 * j);
		string[j + 8] = ecx >> (8 * j);
		string[j + 12] = edx >> (8 * j);
	}
	tty_printf("%s", string);
}

/* AMD-specific information */
int do_amd(void) {
	tty_printf("Detected AMD CPU. AMD-specific features:\n");
	unsigned long extended, eax, ebx, ecx, edx, unused;
	int family, model, stepping, reserved;
	cpuid(1, eax, unused, unused, unused);
	model = (eax >> 4) & 0xf;
	family = (eax >> 8) & 0xf;
	stepping = eax & 0xf;
	reserved = eax >> 12;
	tty_printf("Family: %d Model: %d [", family, model);
	switch(family) {
		case 4:
		tty_printf("486 Model %d", model);
		break;
		case 5:
		switch(model) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 6:
			case 7:
			tty_printf("K6 Model %d", model);
			break;
			case 8:
			tty_printf("K6-2 Model 8");
			break;
			case 9:
			tty_printf("K6-III Model 9");
			break;
			default:
			tty_printf("K5/K6 Model %d", model);
			break;
		}
		break;
		case 6:
		switch(model) {
			case 1:
			case 2:
			case 4:
			tty_printf("Athlon Model %d", model);
			break;
			case 3:
			tty_printf("Duron Model 3");
			break;
			case 6:
			tty_printf("Athlon MP/Mobile Athlon Model 6");
			break;
			case 7:
			tty_printf("Mobile Duron Model 7");
			break;
			default:
			tty_printf("Duron/Athlon Model %d", model);
			break;
		}
		break;
	}
	tty_printf("]\n");
	cpuid(0x80000000, extended, unused, unused, unused);
	if(extended == 0) {
		return 0;
	}
	if(extended >= 0x80000002) {
		unsigned int j;
		tty_printf("Detected Processor Name: ");
		for(j = 0x80000002; j <= 0x80000004; j++) {
			cpuid(j, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		tty_printf("\n");
	}
	if(extended >= 0x80000007) {
		cpuid(0x80000007, unused, unused, unused, edx);
		if(edx & 1) {
			tty_printf("Temperature Sensing Diode Detected!\n");
		}
	}
	tty_printf("Stepping: %d Reserved: %d\n", stepping, reserved);
	return 0;
}
