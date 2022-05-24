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

int end_of_program = 0;

void endSignal(int signal) { //function in order to force stop the communication
	end_of_program = 1;
	printf("\nEnd of the program.\n\n");
}

typedef struct sample {
	double x;
	double y;
	double z;
	double clear;
	double red;
	double green;
	double blue;
} Sample;

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
		error("ERROR opening socket\n");

	server = gethostbyname(argv[1]);

	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	bcopy((char*) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting\n");

	bzero(buffer, 256);
	sprintf(buffer, "Hello Server!");
	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0)
		error("ERROR writing to socket\n");

	bzero(buffer, 256);
	n = read(sockfd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket\n");
	printf("%s\n", buffer);

	ColorSensor *value_color = malloc(sizeof(ColorSensor) * 1); //pointer which reads ACCELEROMETER input
	Acceleration *value_acc = malloc(sizeof(Acceleration) * 1); //pointer which reads COLOR SENSOR input
	Sample value_sample;

	while (end_of_program != 1) {
		signal(SIGINT, endSignal);

		send_socket_message_acc(value_acc);
		send_socket_message_color(value_color);

		value_sample.x = value_acc->x;
		value_sample.y = value_acc->y;
		value_sample.z = value_acc->z;

		value_sample.clear = value_color->clear;
		value_sample.blue = value_color->blue;
		value_sample.green = value_color->green;
		value_sample.red = value_color->red;

		sendto(sockfd, &value_sample, sizeof(Sample), 0, &serv_addr, sizeof(serv_addr));  //send the package to the SERVER

		sleep(10);
	}

	close(sockfd);

	return 0;
}