#ifndef RPI_H
#define RPI_H

#include <stdio.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#define BCM2837_PERI_BASE       0x3F000000
#define GPIO_BASE               (BCM2837_PERI_BASE + 0x200000)	// GPIO controller 

#define BLOCK_SIZE 		(4*1024)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET  *(gpio.addr + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(gpio.addr + 10) // clears bits which are 1 ignores bits which are 0

#define GPIO_READ(g)  *(gpio.addr + 13) &= (1<<(g))

// IO Acces
struct bcm2835_peripheral {
	unsigned long addr_p;
	int mem_fd;
	void* map;
	volatile unsigned int* addr;
};

int map_peripheral(struct bcm2835_peripheral* p);

extern struct bcm2835_peripheral gpio;  // They have to be found somewhere, but can't be in the header

#endif