# **IoT System Environmental Monitoring**
This project involves a data collection node that gathers environmental parameters such as temperature, humidity, and air pressure. The collected data is then transmitted from the node to a central database, and visualized on a website for monitoring purposes. A 4G connection is established using AT commands to enable wireless data transmission.
## Introduction
Below is an overview diagram.
![output](/OverviewDoAn2.png)
Below is the overview hardware.
![output](/Board.jpg)
Below is the Schematic.
![output](/SODONGUYENLY.png)
## Knowledge used in the project
1. Embedded software
* Use HAL library and STM32CubeMX software (**use makefile and VScode editor**)
* Use **UART** protocol to communicate with SIM7600E-H by AT command and Neo-8M GPS
* Use **I2C** protocol to communicate with BMP280 sensor, SHT31 sensor and RTC DS3231.
2. Website Monitoring
![output](/web.jpg)

* Frontend use is ReactJS framework
* Backend use is php programming language
* Database use is mysql
