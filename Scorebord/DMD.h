#ifndef DMD2_H
#define DMD2_H

#include <wiringPi.h>
#include <stddef.h>
#include <string.h>
#include <cstdlib>
#include <iostream>

using namespace std;

// Dimensions of a single display
const unsigned int PANEL_WIDTH = 32;
const unsigned int PANEL_HEIGHT = 16;

class DMDFrame
{
public:
	DMDFrame(unsigned char pixelsWide, unsigned char pixelsHigh);
	virtual ~DMDFrame();

	// Set a single LED on or off
	void setPixel(unsigned int x, unsigned int y, char val);

	void printDisplay();
	// Fill the screen on or off
	void fillScreen(bool on);
	inline void clearScreen() { fillScreen(false); };

	// Drawing primitives
	void drawLine(int x1, int y1, int x2, int y2);
	void drawCircle(unsigned int xCenter, unsigned int yCenter, int radius);
	void drawBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
	void drawFilledBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

	const unsigned char width; // in pixels
	const unsigned char height; // in pixels
protected:
	volatile unsigned char* bitmap;
	unsigned char width_in_panels; // in panels
	unsigned char height_in_panels; // in panels

	// Panels seen as stretched out in a row for purposes of finding index
	inline int pixelToBitmapIndex(unsigned int x, unsigned int y) {
		return (y * width) + x;
	}
};

class DMD : public DMDFrame
{
public:
	DMD(unsigned char panelsWide, unsigned char panelsHigh, unsigned char pin_noe, unsigned char pin_a, unsigned char pin_b, unsigned char pin_sck,
		unsigned char pin_clk, unsigned char pin_r_data);
	void beginNoTimer();
	void scanDisplay();
	void softSPITransfer(unsigned char data);
	inline void setBrightness(unsigned char b){
		brightness = b;
	}
protected:
	void writeSPIData(volatile unsigned char* rows[4], const int rowsize);
private:
	volatile unsigned char scan_row;
	unsigned char pin_noe;
	unsigned char pin_a;
	unsigned char pin_b;
	unsigned char pin_sck;
	unsigned char pin_clk;
	unsigned char pin_r_data;
	unsigned char brightness;
};
#endif