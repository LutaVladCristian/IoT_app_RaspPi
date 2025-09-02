#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "accelerometer.h"
#include "color_sensor.h"

typedef struct sample {
	double x[10];
	double y[10];
	double z[10];
	double clear[10];
	double red[10];
	double green[10];
	double blue[10];
} Sample;


#endif
