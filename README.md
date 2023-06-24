## Client-Server communication using Embedded Linux on Raspberry Pi

##### 1. Introduction:

The aim of the project is to build an IoT application that will use locally configured a
TCP communication between Raspberry Pi3 using a BCM2385 processor
(client) and Linux Ubuntu (server), using the C language.

##### 2. General description:

To configure an embedded Linux operating system ; suitable for the requirements;
we used the Buildroot tool for our Raspberry Pi which we will use as a client in our
communication.
The client will read one set of data per second from two sensors simultaneously
(MPU-6050 & TCS34725) and send 10 measurements at a time as a data packet to the
server running on Linux Ubuntu from a virtual machine.
The server will calculate some experimental characteristics for every 10 data packets
(100 measurements) received from the client.

##### 3. Hardware Design:
Parts list:
- Raspberry Pi
- accelerometer **MPU- 6050**
- colour sensor **TCS**
- wires
- SD card

##### 4. Operating system:

Because we will use the Buildroot tool to configure the operating system, we can
distinguish 3 machines: **build** (virtual machine, where the toolchain is built), **host**
(virtual machine, where the toolchain is executed to create binary files) and **target**
(Raspberry Pi, where we will execute the binary files created by the host).

The C/C++ toolchain obtained for our client will be composed:
- binutils (set of CPU model-specific instructions for generating and
    manipulating binary files)
- kernel libraries
- C/C++ specific libraries
- compilers (gcc/g++)
- debugger (gdb)

After installing Buildroot, in the directory where it is installed, in the terminal we run
the command:
```
$ make menuconfig
```

From this menu we will configure our embedded Linux operating system.
To add a connection to the local Internet network, **mdev support** and
**Broadcom firmware support** for wireless hardware will need to be included in the
configuration. Then edit the files:

- **wpa_supplicant.conf** (which contains the SSID and password of the
    network we want to connect to)
- **./board/raspberrypi3/interfaces** (for adding a wlan0 internet interface)


The complete configuration (including the **mosquitto client** tool and **I2C bus
protocol** support) that we used for this project is in the file
**.config** in the **Buildroot** directory, and to compile it and generate the kernel that will
later be put on an **SD card** (the **client's hard drive**) , we run the commands:
```
$ make
$ sudo dd if=output/images/sdcard.img of=/dev/sdb
```

To handle the client in its terminal we will use an SSH connection based
on the IP assigned to the Raspberry Pi in an Ubuntu terminal.
```
ssh user@ip_of_rasp_pi
```
After booting for the first time you are supposed to be in **root**. For SSH connection you will need to **create another user because you won't be able to connect directly to root**.
Your options for accesing the **client** are to connect peripherals to your Rasp Pi or to set a Putty session using a serial cable. 

##### 5. How to run the code:
a) **server** code must be compiled in Ubuntu terminal as usual:
```
gcc server.c -lm -o server 
./server port_number
```
b) **client** code must be compiled using Embedded Linux cross-compiler in Eclipse. After, the binaries generated must be moved on the ***client*** and executed with the command:
```
./client port_number
``` 


##### 6. ThingsBoard integration:
ThingsBoard is an open-source IoT platform that enables rapid development,
management and scaling of IoT projects.
For setting the ThingsBoard fallow the link:
https://thingsboard.io/docs/user-guide/install/ubuntu/.  
The MQTT protocol provides an easy way to carry messages using a
publish/subscribe model. This makes it suitable for IoT messaging such as: low power
sensors or mobile devices, phones, embedded computers or microcontrollers.  
After installing this platform on the server (Ubuntu), configuring a posgreSQL
database for the variables sent by the client (data read from the sensors), including the
mosquitto publisher protocol in the buildroot configuration; we managed to have a
local integration of our IoT application in the ThingsBoard environment.
This gives you the possibility to graph the data read by the customer in real time.


##### 7. Bibliography:
- https://www.geeksforgeeks.org/socket-programming-cc/
- https://thingsboard.io/docs/user-guide/install/ubuntu/
- https://www.linuxhowtos.org/C_C++/socket.htm
- https://github.com/



