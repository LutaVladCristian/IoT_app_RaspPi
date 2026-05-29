/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int read_full(int fd, void *buf, size_t len) {
	char *cursor = buf;

	while (len > 0) {
		ssize_t bytes_read = recv(fd, cursor, len, 0);
		if (bytes_read < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		if (bytes_read == 0) {
			return 1;
		}

		cursor += bytes_read;
		len -= (size_t) bytes_read;
	}

	return 0;
}

static int write_full(int fd, const void *buf, size_t len) {
	const char *cursor = buf;

	while (len > 0) {
		ssize_t written = write(fd, cursor, len);
		if (written < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		if (written == 0) {
			return -1;
		}

		cursor += written;
		len -= (size_t) written;
	}

	return 0;
}

void error(const char *msg) {
	perror(msg);
	exit(1);
}

typedef struct sample {
	double x[10];
	double y[10];
	double z[10];
	double clear[10];
	double red[10];
	double green[10];
	double blue[10];
} Sample;

double who_max(double v[][10], int n) { //function to determine MAX of 6 measurements
	double max = v[0][0];
	for (int i = 0; i < n; i++)
		for (int j = 0; j < 10; j++)
			if (max < v[i][j])
				max = v[i][j];
	return max;
}

double who_min(double v[][10], int n) { //function to determine MIN of 6 measurements
	double min = v[0][0];
	for (int i = 0; i < n; i++)
		for (int j = 0; j < 10; j++)
			if (v[i][j] < min)
				min = v[i][j];
	return min;
}

float mean_value(double v[][10], int n) { //function to determine MEAN VALUE of 6 measurements
	double sum = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < 10; j++)
			sum += v[i][j];
	return (float) sum / (n*10);
}

float std_value(double v[][10], int n) { //function to determine SANDARD DEVIATION VALUE of 6 measurements
	float mean = mean_value(v, n);
	float sum = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < 10; j++)
			sum += pow(v[i][j] - mean, 2);
	return (float) sqrt(sum / (n*10));
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t n;

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

	bzero(buffer, 256); //SERVER waits for a CLIENT to communicate with
	n = read(newsockfd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket\n");
	if (n == 0) {
		printf("Client disconnected before handshake.\n");
		close(newsockfd);
		close(sockfd);
		return 0;
	}
	buffer[n] = '\0';
	if (strcmp(buffer, "Hello Server!") != 0) { //checks if it is the right message from the CLIENT
		printf("Wrong Message!\n");
		close(newsockfd);
		close(sockfd);
		return 0;
	}
	printf("%s\n\n", buffer);

	if (write_full(newsockfd, "Hello RPI!", strlen("Hello RPI!")) < 0)
		error("ERROR writing to socket\n");

	Sample value_receive[6];
	double acc_x[6][10], acc_y[6][10], acc_z[6][10];
	double clear[6][10], blue[6][10], green[6][10], red[6][10];
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

		int read_status = read_full(newsockfd, &value_receive[i], sizeof(Sample)); //read the package sent by the CLIENT
		if (read_status < 0)
			error("ERROR reading sample from socket\n");
		if (read_status > 0) {
			printf("Client disconnected.\n");
			close(newsockfd);
			close(sockfd);
			return 0;
		}

		for(int j = 0; j < 10; j++)
		{

					acc_x[i][j] = value_receive[i].x[j]; //copy the sample data into several a vectors
					acc_y[i][j] = value_receive[i].y[j];
					acc_z[i][j] = value_receive[i].z[j];

					clear[i][j] = value_receive[i].clear[j];
					blue[i][j] = value_receive[i].blue[j];
					green[i][j] = value_receive[i].green[j];
					red[i][j] = value_receive[i].red[j];

					printf("=====Sample No.%d::%d received!=====\n", i + 1, j + 1);
					printf("acc X = %.1f\n", value_receive[i].x[j]);
					printf("acc Y = %.1f\n", value_receive[i].y[j]);
					printf("acc Z = %.1f\n", value_receive[i].z[j]);

					printf("\n");

					printf("clear: %.1f\n", value_receive[i].clear[j]);
					printf("blue: %.1f\n", value_receive[i].blue[j]);
					printf("green: %.1f\n", value_receive[i].green[j]);
					printf("red: %.1f\n", value_receive[i].red[j]);

					printf("\n");
		}


		i++;
	}

	close(newsockfd);
	close(sockfd);
	return 0;

}
