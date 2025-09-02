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

struct i2c_msg messages_acc[2];
struct i2c_rdwr_ioctl_data packet_acc;

static char write_bytes_acc[w_len_acc];
static char read_bytes_acc[r_len_acc];

static uint8_t slaveAddrAcc = 0x68; // i2cdetect -y 1

typedef struct acc {
	double x, y, z;
} Acceleration;

void send_socket_message_acc(Acceleration *value);
double dataConversion_acc(char MSB, char LSB);
void send_I2C_message_acc(int fd, uint8_t register_addr, uint8_t register_value,
		uint8_t slave_Addr);
void read_I2C_message_acc(int fd, int length, uint8_t register);

#endif
