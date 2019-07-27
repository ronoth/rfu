#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stropts.h>
#include <termios.h>
#include <unistd.h> // to get close() define
#include "include/sleepms.h"

// Make sure you have installed the SiLabs's version of cp210x.c driver
// which has these IOCTLs implemented.
#define IOCTL_GPIOGET		0x8000
#define IOCTL_GPIOSET		0x8001

int main()
{
	int fd;
	printf( "CP210x Serial Test\n");
	fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
		printf( "Error opening port /dev/ttyUSB0\n");
		return 1;
	}
	unsigned short gpioread = 0x00ff;
	unsigned long gpio = 0xff;
	bool even = true;
	while(1) {
		ioctl(fd, IOCTL_GPIOGET, &gpioread);
		printf( "gpio read = %x\n", gpioread);
		if(even) {
			gpio = 0xffff;
		} else {
			gpio = 0x00ff;
		}
		printf( "gpio to write = %x\n", gpio);
		ioctl(fd, IOCTL_GPIOSET, &gpio);
		even = !even;
		sleep_ms(2000);
	}

	close(fd);
	return 0;
}
