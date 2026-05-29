#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>

#define w_len_acc 2
#define r_len_acc 6

typedef struct acc {
	double x, y, z;
} Acceleration;

void send_socket_message_acc(Acceleration *value);
double dataConversion_acc(uint8_t MSB, uint8_t LSB);
int send_I2C_message_acc(int fd, uint8_t register_addr, uint8_t register_value,
		uint8_t slave_Addr);
int read_I2C_message_acc(int fd, int length, uint8_t register_addr);

#endif
