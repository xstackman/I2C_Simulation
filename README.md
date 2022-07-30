# I2C Simulation with GPIOs
## Tested in STM32G0, STM32F7, STM32WB

This implementation is based in the post found [here](https://chowdera.com/2020/11/202011131114268849.html)

## Configuration

This library require two gpios with high or very high speed enabled in the CubeMX.
![alt text](/images/pin_settings.jpg?raw=true)

The library doesn't implement HAL functions to make the switch in logic levels, so you need to change the definitions of SDA and SCL in the IIC.h file to meet your hardware structure. 

You need to check the IDR, ODR and MODER registers to set the correct bits.

