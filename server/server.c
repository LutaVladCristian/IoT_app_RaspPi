/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
	perror(msg);
	exit(1);
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

double who_max(double v[], int n) {
	double max = v[0];
	for (int i = 0; i < n; i++)
		if (max < v[i])
			max = v[i];
	return max;
}

double who_min(double v[], int n) {
	double min = v[0];
	for (int i = 0; i < n; i++)
		if (min > v[i])
			min = v[i];
	return min;
}

float mean_value(double v[], int n) {
	double sum = 0;
	for (int i = 0; i < n; i++)
		sum += v[i];
	return (float) sum / n;
}

float std_value(double v[], int n) {
	float mean = mean_value(v, n);
	float sum = 0;
	for (int i = 0; i < n; i++)
		sum += pow(v[i] - mean, 2);
	return (float) sqrt(sum / n);
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket\n");

	bzero((char*) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding\n");

	listen(sockfd, 1);
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept\n");

	printf("Client awaiting:\n");

	bzero(buffer, 256);
	n = read(newsockfd, buffer, 255);
	if (strcmp(buffer, "Hello Server!") != 0) {
		printf("Wrong Message!\n");
		return 0;
	}
	if (n < 0)
		error("ERROR reading from socket\n");
	printf("%s\n\n", buffer);

	n = write(newsockfd, "Hello RPI!", 18);
	if (n < 0)
		error("ERROR writing to socket\n");

	Sample value_receive[6];
	double acc_x[6], acc_y[6], acc_z[6];
	double clear[6], blue[6], green[6], red[6];
	int i = 0;

	while (1) {

		if (i == 6) {
			printf("+++++After 60s you've got!+++++\n");

			printf("==Acc X:===\n");
			printf("Mean Value = %.1f\n", mean_value(acc_x, i));
			printf("Standard Deviation = %.1f\n", std_value(acc_x, i));
			printf("MIN value = %.1f\n", who_min(acc_x, i));
			printf("MAX value = %.1f\n", who_max(acc_x, i));

			printf("\n");

			printf("==Acc Y:===\n");
			printf("Mean Value = %.1f\n", mean_value(acc_y, i));
			printf("Standard Deviation = %.1f\n", std_value(acc_y, i));
			printf("MIN value = %.1f\n", who_min(acc_y, i));
			printf("MAX value = %.1f\n", who_max(acc_y, i));

			printf("\n");

			printf("==Acc Z:===\n");
			printf("Mean Value = %.1f\n", mean_value(acc_z, i));
			printf("Standard Deviation = %.1f\n", std_value(acc_z, i));
			printf("MIN value = %.1f\n", who_min(acc_z, i));
			printf("MAX value = %.1f\n", who_max(acc_z, i));

			printf("\n");

			printf("==CLEAR:===\n");
			printf("Mean Value = %.1f\n", mean_value(clear, i));
			printf("Standard Deviation = %.1f\n", std_value(clear, i));
			printf("MIN value = %.1f\n", who_min(clear, i));
			printf("MAX value = %.1f\n", who_max(clear, i));

			printf("\n");

			printf("==BLUE:===\n");
			printf("Mean Value = %.1f\n", mean_value(blue, i));
			printf("Standard Deviation = %.1f\n", std_value(blue, i));
			printf("MIN value = %.1f\n", who_min(blue, i));
			printf("MAX value = %.1f\n", who_max(blue, i));

			printf("\n");

			printf("GREEN:===\n");
			printf("Mean Value = %.1f\n", mean_value(green, i));
			printf("Standard Deviation = %.1f\n", std_value(green, i));
			printf("MIN value = %.1f\n", who_min(green, i));
			printf("MAX value = %.1f\n", who_max(green, i));

			printf("\n");

			printf("==RED:===\n");
			printf("Mean Value = %.1f\n", mean_value(red, i));
			printf("Standard Deviation = %.1f\n", std_value(red, i));
			printf("MIN value = %.1f\n", who_min(red, i));
			printf("MAX value = %.1f\n", who_max(red, i));

			printf("\n");

			i = 0;
		}

		recvfrom(newsockfd, &value_receive[i], sizeof(Sample), 0, (struct sockaddr*) &cli_addr, &clilen);
		acc_x[i] = value_receive[i].x;
		acc_y[i] = value_receive[i].y;
		acc_z[i] = value_receive[i].z;

		clear[i] = value_receive[i].clear;
		blue[i] = value_receive[i].blue;
		green[i] = value_receive[i].green;
		red[i] = value_receive[i].red;


		printf("=====Sample No.%d received!=====\n", i+1);
		printf("acc X = %.1f\n", value_receive[i].x);
		printf("acc Y = %.1f\n", value_receive[i].y);
		printf("acc Z = %.1f\n", value_receive[i].z);

		printf("\n");

		printf("clear: %.1f\n", value_receive[i].clear);
		printf("blue: %.1f\n", value_receive[i].blue);
		printf("green: %.1f\n", value_receive[i].green);
		printf("red: %.1f\n", value_receive[i].red);

		printf("\n");

		i++;
	}

	close(newsockfd);
	close(sockfd);
	return 0;

}
