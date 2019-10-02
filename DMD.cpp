#include "DMD.h"

DMD::DMD(unsigned char panelsWide, unsigned char panelsHigh, unsigned char pin_noe, unsigned char pin_a, unsigned char pin_b, unsigned char pin_sck, unsigned char pin_clk, unsigned char pin_r_data)
	:
	DMDFrame(panelsWide* PANEL_WIDTH, panelsHigh* PANEL_HEIGHT),
	scan_row(0),
	pin_noe(pin_noe),
	pin_a(pin_a),
	pin_b(pin_b),
	pin_sck(pin_sck),
	pin_clk(pin_clk),
	pin_r_data(pin_r_data),
	brightness(255)
{
}

void DMD::beginNoTimer()
{
	digitalWrite(pin_clk, LOW);
	pinMode(pin_clk, OUTPUT);

	digitalWrite(pin_r_data, LOW);
	pinMode(pin_r_data, OUTPUT);

	digitalWrite(pin_noe, LOW);
	pinMode(pin_noe, OUTPUT);

	digitalWrite(pin_a, LOW);
	pinMode(pin_a, OUTPUT);

	digitalWrite(pin_b, LOW);
	pinMode(pin_b, OUTPUT);

	digitalWrite(pin_sck, LOW);
	pinMode(pin_sck, OUTPUT);

	clearScreen();
	scanDisplay();
}

void DMD::scanDisplay()
{
	// Rows are send out in 4 blocks of 4 (interleaved), across all panels

	int rowsize = unified_width_bytes();

	volatile unsigned char* rows[4] = {
	  bitmap + (scan_row + 0) * rowsize,
	  bitmap + (scan_row + 4) * rowsize,
	  bitmap + (scan_row + 8) * rowsize,
	  bitmap + (scan_row + 12) * rowsize,
	};

	//Write data
	for (int i = 0; i < rowsize; i++) {
		softSPITransfer(*(rows[3]++));
		softSPITransfer(*(rows[2]++));
		softSPITransfer(*(rows[1]++));
		softSPITransfer(*(rows[0]++));
	}

	digitalWrite(pin_noe, LOW);
	digitalWrite(pin_sck, HIGH); // Latch DMD shift register output
	digitalWrite(pin_sck, LOW); // (Deliberately left as digitalWrite to ensure decent latching time)

	digitalWrite(pin_a, scan_row & 0x01);
	digitalWrite(pin_b, scan_row & 0x02);
	scan_row = (scan_row + 1) % 4;

	if (brightness == 255)
		digitalWrite(pin_noe, HIGH);
	else
		analogWrite(pin_noe, brightness);
}


void DMD::softSPITransfer(unsigned char data) {
	for (unsigned char i = 0; i < 8; i++) {
		unsigned char bit = data & (1 << 7);
		digitalWrite(pin_r_data, bit);
		digitalWrite(pin_clk, 1);
		digitalWrite(pin_clk, 0);
		data <<= 1;
	}
}


