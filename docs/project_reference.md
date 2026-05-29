# Project reference

## Overview
This repository contains a TCP-based sensor sampling application for Raspberry Pi.
The client runs on the Raspberry Pi, reads accelerometer and color sensor values over I2C, groups readings into packets, and sends them to a Linux server.
The server receives the packets, stores sample windows, and computes summary statistics for each window.

## Components

### Client (`client/`)
- `client/client.c` establishes the TCP connection, sends the initial handshake, collects sensor samples, and forwards each packet.
- `client/accelerometer.c` configures the MPU-6050 and converts raw 16-bit acceleration values into engineering units.
- `client/color_sensor.c` configures the TCS34725 color sensor and converts raw color channels into percentages.
- `client/client.h`, `client/accelerometer.h`, and `client/color_sensor.h` declare the shared data structures and sensor APIs.

### Server (`server/`)
- `server/server.c` listens on a TCP port, accepts a client connection, receives `Sample` packets, and computes window statistics.

## Data flow
1. The client opens an I2C device on `/dev/i2c-1` and initializes the MPU-6050 and TCS34725.
2. Every second, the client reads sensor values and stores them into a `Sample` buffer of 10 measurements.
3. After 10 measurements, the client sends the packet to the server.
4. The server receives each packet and accumulates 6 windows (60 samples total) before computing aggregate statistics.

## Reliability notes
- The client now uses stack storage for sensor samples instead of heap allocation, which avoids per-run memory leaks.
- The server now reads TCP payloads using `recv()` and validates the number of bytes received.
- The sensor drivers validate I2C device open/read errors to prevent invalid file descriptor usage.
