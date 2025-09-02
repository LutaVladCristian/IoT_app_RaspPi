#include "color_sensor.h"

void send_socket_message_color(ColorSensor *value) {
	char i2cFile[15];
	int device = 1;
	sprintf(i2cFile, "/dev/i2c-%d", device);
	int fd = open(i2cFile, O_RDWR);

	if (ioctl(fd, I2C_SLAVE, slaveAddrColor) == -1)
		perror("ioctl");

	send_I2C_message_color(fd, ENABLE_REGISTER, 0x03, slaveAddrColor); // We enable AEN and PON so the device starts working.
	send_I2C_message_color(fd, RGBC_TIMING_REGISTER, 0x00, slaveAddrColor); // Internal integration time of 700 ms.
	send_I2C_message_color(fd, WAIT_TIME_REGISTER, 0xFF, slaveAddrColor); // Wait time of 2.4 ms
	send_I2C_message_color(fd, CONTROL_REGISTER, 0x00, slaveAddrColor); // Set gain to 1

	read_I2C_message_color(fd, 1, 0x13); // status register.

	// Now we are going to read the color sensor measurements.
	if ((read_bytes[0]) & 0x01) {
		read_I2C_message_color(fd, 8, 0x14); ////0x14 + 0x80
		value->clear = dataConversion_color(read_bytes[0], read_bytes[1]);
		value->red = dataConversion_color(read_bytes[2], read_bytes[3]);
		value->green = dataConversion_color(read_bytes[4], read_bytes[5]);
		value->blue = dataConversion_color(read_bytes[6], read_bytes[7]);
	} else {
		printf("Error: Data from the sensor could not be read\n");
	}

	close(fd);
}

double dataConversion_color(char MSB, char LSB) {
	uint16_t color_2_bytes = 0;
	double color_percentage = 0;

	color_2_bytes = (MSB << 8 | LSB >> 0);

	color_percentage = (color_2_bytes * 100) / 65536; //(2**16)

	return color_percentage;
}

void send_I2C_message_color(int fd, uint8_t register_addr,
		uint8_t register_value, uint8_t slave_Addr) {
	write_bytes[0] = 0x80 | register_addr;
	write_bytes[1] = register_value;

	messages[0].addr = slave_Addr;
	messages[0].flags = 0;
	messages[0].len = w_len;
	messages[0].buf = write_bytes; // Pointer to the data bytes to be written.

	packet.msgs = messages;
	packet.nmsgs = 1;

	ioctl(fd, I2C_RDWR, &packet); // Send the message:
}

void read_I2C_message_color(int fd, int length, uint8_t register_addr) {
	write_bytes[0] = 0x80 | register_addr;
	messages[0].addr = slaveAddrColor;
	messages[0].flags = 0;
	messages[0].len = 1;           // We define the write operation
	messages[0].buf = write_bytes; // Pointer to the data bytes to be written.

	messages[1].addr = slaveAddrColor;
	messages[1].flags = I2C_M_RD; // We define the read operation
	messages[1].len = length;
	messages[1].buf = read_bytes; // Pointer to the data bytes to be read.

	packet.msgs = messages;
	packet.nmsgs = 2;

	ioctl(fd, I2C_RDWR, &packet); // Send the message:
}
