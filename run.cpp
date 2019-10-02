#include "DMD.h"

int main()
{
	wiringPiSetupPhys();
	DMD dmd(1, 1, 24, 37, 38, 35, 23, 19);
	dmd.setBrightness(100);
	dmd.beginNoTimer();
	dmd.drawBox(0, 0, 31, 15);
	for (;;)
	{
		dmd.scanDisplay();
	}
	return 0;
}

