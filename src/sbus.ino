void checkSBUS()
{
    static uint8_t buffer[25];
    static uint8_t errors = 0;
    static bool failsafe = 0;
    static uint8_t idx;
    static uint8_t lost = 0;
    uint8_t b;
    uint8_t i;
    //word results;

    if (Serial1.available()) {
        b = Serial1.read();

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
                // Decode SBUS packet
                // 25 byte packet received is little endian so we have to swap some bytes.
                channels[1] = ((buffer[1] | buffer[2] << 8) & 0x07FF);
                channels[2] = ((buffer[2] >> 3 | buffer[3] << 5) & 0x07FF);
                channels[3] = ((buffer[3] >> 6 | buffer[4] << 2 | buffer[5] << 10) & 0x07FF);
                channels[4] = ((buffer[5] >> 1 | buffer[6] << 7) & 0x07FF);

                channels[5] = ((buffer[6] >> 4 | buffer[7] << 4) & 0x07FF);
                channels[6] = ((buffer[7] >> 7 | buffer[8] << 1 | buffer[9] << 9) & 0x07FF);
                channels[7] = ((buffer[9] >> 2 | buffer[10] << 6) & 0x07FF);
                channels[8] = ((buffer[10] >> 5 | buffer[11] << 3) & 0x07FF);

                channels[9] = ((buffer[12] | buffer[13] << 8) & 0x07FF);
                channels[10] = ((buffer[13] >> 3 | buffer[14] << 5) & 0x07FF);
                channels[11] = ((buffer[14] >> 6 | buffer[15] << 2 | buffer[16] << 10) & 0x07FF);
                channels[12] = ((buffer[16] >> 1 | buffer[17] << 7) & 0x07FF);

                channels[13] = ((buffer[17] >> 4 | buffer[18] << 4) & 0x07FF);
                channels[14] = ((buffer[18] >> 7 | buffer[19] << 1 | buffer[20] << 9) & 0x07FF);
                channels[15] = ((buffer[20] >> 2 | buffer[21] << 6) & 0x07FF);
                channels[16] = ((buffer[21] >> 5 | buffer[22] << 3) & 0x07FF);

                channels[17] = ((buffer[23]) & 0x0001) ? 2047 : 0;
                channels[18] = ((buffer[23] >> 1) & 0x0001) ? 2047 : 0;

                // Check for failsafe from receiver
                failsafe = ((buffer[23] >> 3) & 0x0001) ? 1 : 0;

                // Handle incomplete packages
                if ((buffer[23] >> 2) & 0x0001)
                    lost++;
            }
        }
    }
}
