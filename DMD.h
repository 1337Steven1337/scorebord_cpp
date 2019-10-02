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

// Clamp a value between two limits
template<typename T> static inline void clamp(T& value, T lower, T upper) {
	if (value < lower)
		value = lower;
	else if (value > upper)
		value = upper;
}

// Swap A & B "in place" (well, with a temp variable!)
template<typename T> static inline void swap(T& a, T& b)
{
	T tmp(a); a = b; b = tmp;
}

// Check a<=b, and swap them otherwise
template<typename T> static inline void ensureOrder(T& a, T& b)
{
	if (b < a) swap(a, b);
}

extern const unsigned char DMD_Pixel_Lut[];


//Pixel/graphics writing modes
enum DMDGraphicsMode {
	GRAPHICS_OFF, // unconditionally off (pixel turns off)
	GRAPHICS_ON, //unconditionally on (pixel turns on, the usual default for drawing)
	GRAPHICS_INVERSE, // on if was going to set to off
	GRAPHICS_OR, // add to pixels already on
	GRAPHICS_NOR, // subtract from pixels already on, don't turn any new ones on
	GRAPHICS_XOR, // swap on/off state of pixels
	GRAPHICS_NOOP // No-Op, ie don't actually change anything
};

// Return the inverse/"clear" version of the given mode
// ie for normal pixel-on modes, the "clear" is to turn off.
// for inverse mode, it's to turn on.
// for all other modes, this is kind of meaningless so we return a no-op
inline static DMDGraphicsMode inverseMode(DMDGraphicsMode mode) {
	switch (mode) {
	case GRAPHICS_ON:
		return GRAPHICS_OFF;
	case GRAPHICS_INVERSE:
		return GRAPHICS_ON;
	default:
		return GRAPHICS_NOOP;
	}
};


class DMDFrame
{
public:
	DMDFrame(unsigned char pixelsWide, unsigned char pixelsHigh);
	DMDFrame(const DMDFrame& source);
	virtual ~DMDFrame();

	// Set a single LED on or off
	void setPixel(unsigned int x, unsigned int y, DMDGraphicsMode mode = GRAPHICS_ON);

	// Get status of a single LED
	bool getPixel(unsigned int x, unsigned int y);

	//Print display
	void printDisplay();

	// Move a region of pixels from one area to another
	void movePixels(unsigned int from_x, unsigned int from_y,
		unsigned int to_x, unsigned int to_y,
		unsigned int width, unsigned int height);

	// Extract a sub-region of the frame as a new frame
	DMDFrame subFrame(unsigned int left, unsigned int top, unsigned int width, unsigned int height);

	// Copy the contents of another frame back into this one at the given location
	void copyFrame(DMDFrame& from, unsigned int left, unsigned int top);

	// Fill the screen on or off
	void fillScreen(bool on);
	inline void clearScreen() { fillScreen(false); };

	// Drawing primitives
	void drawLine(int x1, int y1, int x2, int y2, DMDGraphicsMode mode = GRAPHICS_ON);
	void drawCircle(unsigned int xCenter, unsigned int yCenter, int radius, DMDGraphicsMode mode = GRAPHICS_ON);
	void drawBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, DMDGraphicsMode mode = GRAPHICS_ON);
	void drawFilledBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, DMDGraphicsMode mode = GRAPHICS_ON);

	// Scrolling & marquee support
	void scrollY(int scrollBy);
	void scrollX(int scrollBy);
	void marqueeScrollX(int scrollBy);
	void marqueeScrollY(int scrollBy);

	void swapBuffers(DMDFrame& other);

	const unsigned char width; // in pixels
	const unsigned char height; // in pixels
protected:
	volatile unsigned char* bitmap;
	unsigned char row_width_bytes; // width in bitmap, bit-per-pixel rounded up to nearest byte
	unsigned char height_in_panels; // in panels

	unsigned char* font;

	// total bytes in the bitmap
	inline size_t bitmap_bytes() {
		return row_width_bytes * height;
	}
	// controller sees all panels as end-to-end, so bitmap arranges it that way
	inline size_t unified_width_bytes() {
		return row_width_bytes * height_in_panels;
	}

	// Panels seen as stretched out in a row for purposes of finding index
	inline int pixelToBitmapIndex(unsigned int x, unsigned int y) {
		unsigned char panel = (x / PANEL_WIDTH) + ((width / PANEL_WIDTH) * (y / PANEL_HEIGHT));
		x = (x % PANEL_WIDTH) + (panel * PANEL_WIDTH);
		y = y % PANEL_HEIGHT;
		int res = x / 8 + (y * unified_width_bytes());
		return res;
	}
	template<typename T> inline void clamp_xy(T& x, T& y) {
		clamp(x, (T)0, (T)width - 1);
		clamp(y, (T)0, (T)width - 1);
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
