#include "DMD.h"

int main()
{
	wiringPiSetupPhys();
	DMD dmd(7, 1, 24, 37, 38, 35, 23, 19);
	dmd.setBrightness(255);
	dmd.beginNoTimer();
	dmd.drawBox(0, 0, 223, 15);
	for (;;)
	{
		dmd.scanDisplay();
	}
	return 0;
}
