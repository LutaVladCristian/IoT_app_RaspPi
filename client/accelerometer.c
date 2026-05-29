#include "accelerometer.h"

static const uint8_t slaveAddrAcc = 0x68; // i2cdetect -y 1
static struct i2c_msg messages_acc[2];
static struct i2c_rdwr_ioctl_data packet_acc;
static uint8_t write_bytes_acc[w_len_acc];
static uint8_t read_bytes_acc[r_len_acc];

void send_socket_message_acc(Acceleration *value) {
	char i2cFile[15];
	int device = 1;
	snprintf(i2cFile, sizeof(i2cFile), "/dev/i2c-%d", device);
	int fd = open(i2cFile, O_RDWR);

	if (fd < 0) {
		perror("open");
		return;
	}

	if (ioctl(fd, I2C_SLAVE, slaveAddrAcc) == -1) {
		perror("ioctl");
		close(fd);
		return;
	}

	if (send_I2C_message_acc(fd, 0x6B, 0x01, slaveAddrAcc) < 0 || // Power management register
			send_I2C_message_acc(fd, 0x38, 0x01, slaveAddrAcc) < 0 || // Interrupt enable register.
			send_I2C_message_acc(fd, 0x19, 0x07, slaveAddrAcc) < 0 || // Sample rate divider register.
			send_I2C_message_acc(fd, 0x1A, 0x00, slaveAddrAcc) < 0 || // Configuration register
			send_I2C_message_acc(fd, 0x23, 0x00, slaveAddrAcc) < 0 || // Disable FIFO for real-time measurements
			send_I2C_message_acc(fd, 0x1C, 0x08, slaveAddrAcc) < 0) { // Set scale to +- 4g
		close(fd);
		return;
	}

	if (read_I2C_message_acc(fd, r_len_acc, 0X3B) < 0) {
		close(fd);
		return;
	}

	value->x = dataConversion_acc(read_bytes_acc[0], read_bytes_acc[1]);
	value->y = dataConversion_acc(read_bytes_acc[2], read_bytes_acc[3]);
	value->z = dataConversion_acc(read_bytes_acc[4], read_bytes_acc[5]);

	close(fd);
}

double dataConversion_acc(uint8_t MSB, uint8_t LSB) {
	uint16_t acceleration_aux = 0;
	double acceleration = 0;

	acceleration_aux = ((uint16_t) MSB << 8) | LSB;

	if (acceleration_aux >= 0x8000) {
		acceleration_aux = (~acceleration_aux) + 1;
		acceleration = -1 * (double) acceleration_aux / (8192);
	} else {
		acceleration = (double) acceleration_aux / (8192);
	}

	return acceleration;
}

int send_I2C_message_acc(int fd, uint8_t register_addr, uint8_t register_value,
		uint8_t slave_Addr) {
	write_bytes_acc[0] = register_addr;
	write_bytes_acc[1] = register_value;

	messages_acc[0].addr = slave_Addr; // 0x68
	messages_acc[0].flags = 0;
	messages_acc[0].len = w_len_acc;
	messages_acc[0].buf = write_bytes_acc; // Pointer to the data bytes to be written.

	packet_acc.msgs = messages_acc;
	packet_acc.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &packet_acc) < 0) {
		perror("ioctl I2C_RDWR write accelerometer");
		return -1;
	}

	return 0;
}

int read_I2C_message_acc(int fd, int length, uint8_t register_addr) {
	if (length > r_len_acc) {
		fprintf(stderr, "accelerometer read length too large: %d\n", length);
		return -1;
	}

	write_bytes_acc[0] = register_addr;
	messages_acc[0].addr = slaveAddrAcc;
	messages_acc[0].flags = 0;
	messages_acc[0].len = 1;           // We define the read operation
	messages_acc[0].buf = write_bytes_acc; // Pointer to the data bytes to be written.

	messages_acc[1].addr = slaveAddrAcc;
	messages_acc[1].flags = I2C_M_RD; // We define the read operation
	messages_acc[1].len = length;
	messages_acc[1].buf = read_bytes_acc; // Pointer to the data bytes to be written.

	packet_acc.msgs = messages_acc;
	packet_acc.nmsgs = 2;

	if (ioctl(fd, I2C_RDWR, &packet_acc) < 0) {
		perror("ioctl I2C_RDWR read accelerometer");
		return -1;
	}

	return 0;
}
