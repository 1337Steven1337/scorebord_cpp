#include "RPI.h"
#include "DMD.h"

int main()
{
	if (map_peripheral(&gpio) == -1)
	{
		printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
		return -1;
	}
	DMD dmd(1, 1, 18, 4, 14, 17, 27, 3); // oe, a, b, clk, strobe/latch, data
	dmd.setBrightness(100);
	dmd.drawBox(0, 0, 31, 15);
	for (;;)
	{
		for (int i = 0; i < 4; i++) {
			dmd.scanDisplay();
		}
	}
	return 0;
}

