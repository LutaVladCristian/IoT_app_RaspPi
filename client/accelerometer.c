#include "accelerometer.h"

void send_socket_message_acc(Acceleration *value) {
	char i2cFile[15];
	int device = 1;
	sprintf(i2cFile, "/dev/i2c-%d", device);
	int fd = open(i2cFile, O_RDWR);

	if (ioctl(fd, I2C_SLAVE, slaveAddrAcc) == -1)
		perror("ioctl");

	send_I2C_message_acc(fd, 0x6B, 0x01, slaveAddrAcc); // Power management register
	send_I2C_message_acc(fd, 0x38, 0x01, slaveAddrAcc); // Interrupt enable register.
	send_I2C_message_acc(fd, 0x19, 0x07, slaveAddrAcc); // Sample rate divider register.
	send_I2C_message_acc(fd, 0x1A, 0x00, slaveAddrAcc); // Configuration register
	send_I2C_message_acc(fd, 0x23, 0x00, slaveAddrAcc); // Disable the FIFO so we can have real time measurements
	send_I2C_message_acc(fd, 0x1C, 0x08, slaveAddrAcc); // Set the scale to +- 4g

	read_I2C_message_acc(fd, 8, 0X3B);

	value->x = dataConversion_acc(read_bytes_acc[0], read_bytes_acc[1]);
	value->y = dataConversion_acc(read_bytes_acc[2], read_bytes_acc[3]);
	value->z = dataConversion_acc(read_bytes_acc[4], read_bytes_acc[5]);

	close(fd);
}

double dataConversion_acc(char MSB, char LSB) {
	uint16_t acceleration_aux = 0;
	double acceleration = 0;

	acceleration_aux = (MSB << 8 | LSB >> 0);

	if (acceleration_aux >= 0x8000) {
		acceleration_aux = (~acceleration_aux) + 1;
		acceleration = -1 * (double) acceleration_aux / (8192);
	} else {
		acceleration = (double) acceleration_aux / (8192);
	}

	return acceleration;
}

void send_I2C_message_acc(int fd, uint8_t register_addr, uint8_t register_value,
		uint8_t slave_Addr) {
	write_bytes_acc[0] = register_addr;
	write_bytes_acc[1] = register_value;

	messages_acc[0].addr = slave_Addr; // 0x68
	messages_acc[0].flags = 0;
	messages_acc[0].len = w_len_acc;
	messages_acc[0].buf = write_bytes_acc; // Pointer to the data bytes to be written.

	packet_acc.msgs = messages_acc;
	packet_acc.nmsgs = 1;

	ioctl(fd, I2C_RDWR, &packet_acc); // Send the message:
}

void read_I2C_message_acc(int fd, int length, uint8_t register_addr) {
	write_bytes_acc[0] = 0x3B;
	messages_acc[0].addr = slaveAddrAcc;
	messages_acc[0].flags = 0;
	messages_acc[0].len = 1;           // We define the read operation
	messages_acc[0].buf = write_bytes_acc; // Pointer to the data bytes to be written.

	messages_acc[1].addr = slaveAddrAcc;
	messages_acc[1].flags = I2C_M_RD; // We define the read operation
	messages_acc[1].len = 6;
	messages_acc[1].buf = read_bytes_acc; // Pointer to the data bytes to be written.

	packet_acc.msgs = messages_acc;
	packet_acc.nmsgs = 2;

	ioctl(fd, I2C_RDWR, &packet_acc); // Send the message:
}
