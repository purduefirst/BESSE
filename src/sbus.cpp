#include "HardwareSerial.h"
#include "debug.hpp"
#include "Arduino.h"

#define SBUS_BAUD 100000

/**
 * Class representing the SBUS port.
 */
class Sbus {
  public:
  
	// Public Variables
	
	bool failsafe = 0;
	
	
	// Public Functions
	
	/**
	 * Class constructor. Sets _UART to the desired Serial Port.
	 *
	 * @param UART Pointer to the Serial Port.
	 */
    Sbus(HardwareSerial &UART) {
		// Initialize SBUS UART
		UART.begin(SBUS_BAUD, SERIAL_8E2);
	  
		// Wait until UART initializes
		while (!UART.available());
		
		debugPrint("SBUS UART Initialized!");
	  
		_UART = &UART;
	  
	}

	/**
	 * Poll the Serial Port for new bytes and construct an SBUS packet. 
	 *     This populates the _channels array with controller values. It
	 *	   sets a failsafe flag upon receiving a failsafe signal from
	 *	   the receiver. It logs failed packets in the errors variable.
	 */
	void read(){
		if (_UART->available()) {
			b = _UART->read();

			// Wait for start byte (MSB so 11110000 = 0x0F)
			if (idx == 0 && b != 0x0F) { 
				// Do nothing
			}
			else {
				// Read until end byte is received
				buffer[idx++] = b; 
			}

			// Check that all 25 bytes have been read
			if (idx == 25) {
				idx = 0;

				// Check that there is a stop byte
				if (buffer[24] != 0x00) {
					errors++;
				}
				else {
					//Decode received packet
					decode();
				}
			}
		}
	}

	/**
	 * Retrieves latest value from the _channels array.
	 *
	 * @param channel Array index of desired channel.
	 * @return int Latest received value from receiver.
	 */
	int getChannel(uint8_t channel){
		// TODO: Do a little parsing here to verify actual channel id
		return _channels[channel];
	}
  
  
	private:
	
		// Private Variables
		
		HardwareSerial *_UART;
		int _channels[18] = {0};
		uint8_t buffer[25] = {0};
		uint8_t errors = 0;
		uint8_t idx = 0;
		uint8_t lost = 0;
		uint8_t b = 0;
		uint8_t i = 0;
		
		
		// Private Functions
		
		/**
		 * Decodes an SBUS packet into an array of channels. Sets failsafe flag.
		 */
		void decode(){
			// Decode SBUS packet
			// 25 byte packet received is little endian so we have to swap some bytes.
			_channels[1] = ((buffer[1] | buffer[2] << 8) & 0x07FF);
			_channels[2] = ((buffer[2] >> 3 | buffer[3] << 5) & 0x07FF);
			_channels[3] = ((buffer[3] >> 6 | buffer[4] << 2 | buffer[5] << 10) & 0x07FF);
			_channels[4] = ((buffer[5] >> 1 | buffer[6] << 7) & 0x07FF);

			_channels[5] = ((buffer[6] >> 4 | buffer[7] << 4) & 0x07FF);
			_channels[6] = ((buffer[7] >> 7 | buffer[8] << 1 | buffer[9] << 9) & 0x07FF);
			_channels[7] = ((buffer[9] >> 2 | buffer[10] << 6) & 0x07FF);
			_channels[8] = ((buffer[10] >> 5 | buffer[11] << 3) & 0x07FF);

			_channels[9] = ((buffer[12] | buffer[13] << 8) & 0x07FF);
			_channels[10] = ((buffer[13] >> 3 | buffer[14] << 5) & 0x07FF);
			_channels[11] = ((buffer[14] >> 6 | buffer[15] << 2 | buffer[16] << 10) & 0x07FF);
			_channels[12] = ((buffer[16] >> 1 | buffer[17] << 7) & 0x07FF);

			_channels[13] = ((buffer[17] >> 4 | buffer[18] << 4) & 0x07FF);
			_channels[14] = ((buffer[18] >> 7 | buffer[19] << 1 | buffer[20] << 9) & 0x07FF);
			_channels[15] = ((buffer[20] >> 2 | buffer[21] << 6) & 0x07FF);
			_channels[16] = ((buffer[21] >> 5 | buffer[22] << 3) & 0x07FF);

			_channels[17] = ((buffer[23]) & 0x0001) ? 2047 : 0;
			_channels[18] = ((buffer[23] >> 1) & 0x0001) ? 2047 : 0;

			// Check for failsafe from receiver
			failsafe = ((buffer[23] >> 3) & 0x0001) ? 1 : 0;

			// Handle incomplete packages
			if ((buffer[23] >> 2) & 0x0001)
				lost++;
		}
};

