#include "DMD2.h"

DMD::DMD(byte panelsWide, byte panelsHigh, byte pin_noe, byte pin_a, byte pin_b, byte pin_sck, byte pin_clk, byte pin_r_data)
	:
	DMDFrame(panelsWide* PANEL_WIDTH, panelsHigh* PANEL_HEIGHT),
	scan_row(0),
	pin_noe(pin_noe),
	pin_a(pin_a),
	pin_b(pin_b),
	pin_clk(pin_clk),
	pin_sck(pin_sck),
	pin_r_data(pin_r_data),
	default_pins(pin_noe == 9 && pin_a == 6 && pin_b == 7 && pin_sck == 8),
	pin_other_cs(-1),
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
	if (pin_other_cs >= 0 && digitalRead(pin_other_cs) != HIGH)
		return;
	// Rows are send out in 4 blocks of 4 (interleaved), across all panels

	int rowsize = unified_width_bytes();

	volatile uint8_t* rows[4] = { 
	  bitmap + (scan_row + 0) * rowsize,
	  bitmap + (scan_row + 4) * rowsize,
	  bitmap + (scan_row + 8) * rowsize,
	  bitmap + (scan_row + 12) * rowsize,
	};

	writeSPIData(rows, rowsize);

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


static inline __attribute__((always_inline)) void softSPITransfer(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t bit = ((data & (1 << i)) == 0);
		digitalWrite(pin_r_data, bit);
		digitalWrite(pin_clk, 1);
		digitalWrite(pin_clk, 0);
	}
}

void DMD::writeSPIData(volatile uint8_t* rows[4], const int rowsize)
{
	for (int i = 0; i < rowsize; i++) {
		softSPITransfer(*(rows[3]++));
		softSPITransfer(*(rows[2]++));
		softSPITransfer(*(rows[1]++));
		softSPITransfer(*(rows[0]++));
	}
}


