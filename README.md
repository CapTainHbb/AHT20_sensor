# AHT20_sensor
project Components:
* AHT20 humidity & temperature sensor 
* STM32F103C8 as Micro-controller
* nokia5110 LCD

recommended port-pin mapping:

**NOKIA5110**

SPI1 FOR SPI(Transmit Only Master)

PA5  ----> SPI1_SCK 

PA7  ----> SPI1_MOSI

PB1  ----> NOKIA CE

PB10 ----> NOKIA RST

PB11 ----> NOKIA DC






**AHT20**

I2C1 FOR I2C

PB7 ----> I2C1_SDA

PB6 ----> I2C1_SCL


