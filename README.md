## Client-Server Communication using Embedded Linux on Raspberry Pi

### 1. Introduction

This project aims to build an IoT application using locally configured TCP communication between a Raspberry Pi 3 (with a BCM2385 processor, acting as the **client**) and a Linux Ubuntu machine (acting as the **server**). Both client and server applications are written in C.

---

### 2. General Description

To configure an embedded Linux operating system suitable for the project’s requirements, we used the **Buildroot** tool for the Raspberry Pi, which functions as the client in our setup.  

The client reads one set of data per second from two sensors simultaneously:  
- **MPU-6050** (accelerometer/gyroscope)  
- **TCS34725** (color sensor)  

It groups 10 measurements into a single data packet and sends this packet to the server running on Ubuntu inside a virtual machine.  

The server processes incoming data and calculates experimental characteristics for every 10 data packets (i.e., 100 measurements).

---

### 3. Hardware Design

**Parts list:**
- Raspberry Pi 3  
- Accelerometer: **MPU-6050**  
- Color sensor: **TCS34725**  
- Wires and connectors  
- SD card  

---

### 4. Operating System Setup

Using **Buildroot**, we distinguish three machines in the workflow:  
- **Build machine** – virtual machine where the toolchain is built.  
- **Host machine** – virtual machine where the toolchain is executed to generate binary files.  
- **Target machine** – Raspberry Pi, where binaries are deployed and executed.  

The toolchain for the client includes:  
- **binutils** – CPU-specific tools for generating and manipulating binaries  
- **Kernel libraries**  
- **C/C++ libraries**  
- **Compilers (gcc/g++)**  
- **Debugger (gdb)**  

#### Configuring Buildroot
After installing Buildroot, navigate to its directory and run:
```bash
make menuconfig
```

From this menu, configure the embedded Linux OS.  
To enable network connectivity, include **mdev support** and **Broadcom firmware support**.  

Then configure:  
- **wpa_supplicant.conf** – stores Wi-Fi SSID and password.  
- **./board/raspberrypi3/interfaces** – adds a `wlan0` network interface.  

The complete configuration (including the **mosquitto client** and **I2C bus support**) is stored in the `.config` file inside the Buildroot directory.  
To compile and generate the kernel image for the SD card (used as the client’s storage), run:  
```bash
make
sudo dd if=output/images/sdcard.img of=/dev/sdb
```

#### Accessing the Raspberry Pi
You can connect via SSH using its assigned IP address:
```bash
ssh user@ip_of_rasp_pi
```

> **Note:** After the first boot, you will be logged in as **root**. Since SSH does not allow direct root login, you must create a new user account.  
> You may also connect peripherals directly to the Raspberry Pi or use a **PuTTY session with a serial cable**.

---

### 5. Running the Code

**a) Server (on Ubuntu):**
```bash
gcc server.c -lm -o server 
./server port_number
```

**b) Client (on Raspberry Pi):**  
Compile the code using the cross-compiler in Eclipse. After generating the binaries, copy them to the client device and run:
```bash
./client port_number
```

---

### 6. ThingsBoard Integration

[ThingsBoard](https://thingsboard.io/docs/user-guide/install/ubuntu/) is an open-source IoT platform that enables rapid development, management, and scaling of IoT projects.  

We integrated ThingsBoard to visualize and manage sensor data.  

- **MQTT protocol** was used for lightweight publish/subscribe messaging.  
- **PostgreSQL database** was configured to store variables sent by the client.  
- **Mosquitto client** was enabled in the Buildroot configuration.  

With this setup, our IoT application can send sensor data from the client to ThingsBoard, where it is stored, managed, and **graphed in real time**.

---

### 7. Communication Flow Diagram

The following diagram summarizes the system communication flow:

<img width="768" height="100" alt="communication_flow" src="https://github.com/user-attachments/assets/a9c9b0b3-705c-4717-a60b-90cf25a0d054" />

---

### 8. Bibliography:
- https://www.geeksforgeeks.org/socket-programming-cc/
- https://thingsboard.io/docs/user-guide/install/ubuntu/
- https://www.linuxhowtos.org/C_C++/socket.htm
- https://github.com/
