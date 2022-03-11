# AHT20_sensor
project Components:
* AHT20 humidity & temperature sensor 
* STM32F103C8 as Micro-controller
* nokia5110 LCD

I strongly suggest that first read the datasheet of sensor(https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf)

### recommended port-pin mapping

##### NOKIA5110

SPI1 FOR SPI(Transmit Only Master)

```PA5  ----> SPI1_SCK

PA7  ----> SPI1_MOSI

PB1  ----> NOKIA CE

PB10 ----> NOKIA RST

PB11 ----> NOKIA DC
```






##### AHT20

I2C1 FOR I2C
```
PB7 ----> I2C1_SDA

PB6 ----> I2C1_SCL
```

