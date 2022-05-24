#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>

#define w_len 2
#define r_len 8

#define ENABLE_REGISTER 0x00
#define RGBC_TIMING_REGISTER 0X01
#define WAIT_TIME_REGISTER 0x03
#define CONTROL_REGISTER 0X0F

struct i2c_msg messages[2];
struct i2c_rdwr_ioctl_data packet;

static char write_bytes[w_len];
static char read_bytes[r_len];

static uint8_t slaveAddrColor = 0x29; // i2cdetect -y 1

typedef struct light {
	double clear;
	double red;
	double green;
	double blue;
} ColorSensor;

void send_socket_message_color(ColorSensor *value);
double dataConversion_color(char MSB, char LSB);
void read_I2C_message_color(int fd, int length, uint8_t register);
void send_I2C_message_color(int fd, uint8_t register_addr,
		uint8_t register_value, uint8_t slave_Addr);

#endif
