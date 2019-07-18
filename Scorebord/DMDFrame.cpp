#include "DMD.h"

DMDFrame::DMDFrame(unsigned char pixelsWide, unsigned char pixelsHigh)
	:
	width(pixelsWide),
	height(pixelsHigh)
{
	width_in_panels = (pixelsWide + PANEL_WIDTH - 1) / PANEL_WIDTH;
	height_in_panels = (pixelsHigh + PANEL_HEIGHT - 1) / PANEL_HEIGHT;
	bitmap = (unsigned char*)malloc(width * height * sizeof(char));
	clearScreen();
}


DMDFrame::~DMDFrame()
{
	free((void*)bitmap);
}

void DMDFrame::setPixel(unsigned int x, unsigned int y, char val)
{
	if (x >= width || y >= height)
		return;

	int byte_idx = pixelToBitmapIndex(x, y);
	*(bitmap + byte_idx * sizeof(char)) = val;
}

void DMDFrame::printDisplay()
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int byte_idx = pixelToBitmapIndex(x, y);
			int val = *(bitmap + byte_idx * sizeof(char));
			std::cout << val;
		}
		std::cout << std::endl;
	}
}

void DMDFrame::fillScreen(bool on)
{
	memset((void*)bitmap, on ? 0 : 1, width*height*sizeof(char));
}

void DMDFrame::drawLine(int x1, int y1, int x2, int y2)
{
	int dy = y2 - y1;
	int dx = x2 - x1;
	int stepx, stepy;

	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	}
	else {
		stepy = 1;
	}
	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	}
	else {
		stepx = 1;
	}
	dy = dy * 2;
	dx = dx * 2;

	setPixel(x1, y1, 0);
	if (dx > dy) {
		int fraction = dy - (dx / 2);	// same as 2*dy - dx
		while (x1 != x2) {
			if (fraction >= 0) {
				y1 += stepy;
				fraction -= dx;	// same as fraction -= 2*dx
			}
			x1 += stepx;
			fraction += dy;	// same as fraction -= 2*dy
			setPixel(x1, y1, 0);
		}
	}
	else {
		int fraction = dx - (dy / 2);
		while (y1 != y2) {
			if (fraction >= 0) {
				x1 += stepx;
				fraction -= dy;
			}
			y1 += stepy;
			fraction += dx;
			setPixel(x1, y1, 0);
		}
	}
}

void DMDFrame::drawCircle(unsigned int xCenter, unsigned int yCenter, int radius)
{
	// Bresenham's circle drawing algorithm
	int x = -radius;
	int y = 0;
	int error = 2 - 2 * radius;
	while (x < 0) {
		setPixel(xCenter - x, yCenter + y, 0);
		setPixel(xCenter - y, yCenter - x, 0);
		setPixel(xCenter + x, yCenter - y, 0);
		setPixel(xCenter + y, yCenter + x, 0);
		radius = error;
		if (radius <= y) error += ++y * 2 + 1;
		if (radius > x || error > y) error += ++x * 2 + 1;
	}
}

void DMDFrame::drawBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	drawLine(x1, y1, x2, y1);
	drawLine(x2, y1, x2, y2);
	drawLine(x2, y2, x1, y2);
	drawLine(x1, y2, x1, y1);
}

void DMDFrame::drawFilledBox(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	for (unsigned int b = x1; b <= x2; b++) {
		drawLine(b, y1, b, y2);
	}
}