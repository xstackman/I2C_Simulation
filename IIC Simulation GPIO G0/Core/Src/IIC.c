/*
 * IIC.c
 *
 *  Created on: Jul 24, 2022
 *      Author: emisario
 */

#include "IIC.h"
#include "stm32g0xx_hal.h"

void SCL_Delay(uint8_t t) {
	/* counter reset to 0x00, TOF is cleared*/
	unsigned int i;
	for (i = 0; i < 30; i++) {
		asm("NOP");
	}
}

void IIC_Init(void) {


	IIC_SCL(1);
	IIC_SDA(1);
}
// produce IIC Start signal
void IIC_Start(void) {

	SDA_OUT();     //sda Line out
	IIC_SDA(1);
	IIC_SCL(1);
	SCL_Delay(4);
	IIC_SDA(0);     //START:when CLK is high,DATA change form high to low
	SCL_Delay(4);
	IIC_SCL(0);     // Hold on I2C Bus , Ready to send or receive data
}
// produce IIC Stop signal
void IIC_Stop(void) {

	SDA_OUT();     //sda Line out
	IIC_SCL(0);
	IIC_SDA(0);     //STOP:when CLK is high DATA change form low to high
	SCL_Delay(4);
	IIC_SCL(1);
	IIC_SDA(1);     // send out I2C Bus end signal
	SCL_Delay(4);
}
// Waiting for the answer signal to arrive
// Return value ：1, Failed to receive response
//        0, Received response successfully
uint8_t IIC_Wait_Ack(void) {

	uint8_t ucErrTime = 0;
	SDA_IN();      //SDA Set to input
	IIC_SDA(1);
	SCL_Delay(1);
	IIC_SCL(1);
	SCL_Delay(1);
	while (READ_SDA()) {

		ucErrTime++;
		if (ucErrTime > 250) {

			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);      // Clock output 0
	return 0;
}
// produce ACK The reply
void IIC_Ack(void) {

	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	SCL_Delay(2);
	IIC_SCL(1);
	SCL_Delay(2);
	IIC_SCL(0);
}
// Do not produce ACK The reply
void IIC_NAck(void) {

	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	SCL_Delay(2);
	IIC_SCL(1);
	SCL_Delay(2);
	IIC_SCL(0);
}
//IIC Send a byte
// Return whether the slave has a response
//1, There's a response
//0, No response
void IIC_Send_Byte(uint8_t txd) {

	uint8_t t;
	SDA_OUT();
	IIC_SCL(0);      // Pull down the clock and start data transmission
	for (t = 0; t < 8; t++) {

		IIC_SDA((txd & 0x80) >> 7);
		txd <<= 1;
		SCL_Delay(2);   // Yes TEA5767 All three delays are necessary
		IIC_SCL(1);
		SCL_Delay(2);
		IIC_SCL(0);
		SCL_Delay(2);
	}
}
// read 1 Bytes ,ack=1 when , send out ACK,ack=0, send out nACK
uint8_t IIC_Read_Byte(unsigned char ack) {

	unsigned char i, receive = 0;
	SDA_IN();   //SDA Set to input
	for (i = 0; i < 8; i++) {

		IIC_SCL(0);
		SCL_Delay(2);
		IIC_SCL(1);
		receive <<= 1;
		if (READ_SDA())
			receive++;
		SCL_Delay(1);
	}
	if (!ack)
		IIC_NAck();   // send out nACK
	else
		IIC_Ack(); // send out ACK
	return receive;
}

void IIC_SCL(uint8_t val) {
	if (val) {
		GPIOA->ODR |= (1 << 9);
	} else {
		GPIOA->ODR &= ~(1 << 9);
	}

}
void IIC_SDA(uint8_t val) {
	if (val) {
		GPIOC->ODR |= (1 << 7);
	} else {
		GPIOC->ODR &= ~(1 << 7);
	}
}
uint8_t READ_SDA() {

	return ((uint8_t) ((GPIOC->IDR >> 7) & 1));
}

void IIC_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data) {
	IIC_Start();  //1

	IIC_Send_Byte(daddr);	    // Send write command 		2
	IIC_Wait_Ack();	    //3
	IIC_Send_Byte(addr);	    // Sending address 	4
	IIC_Wait_Ack();	   	 	//5
	IIC_Send_Byte(data);     // Send byte 				6
	IIC_Wait_Ack();  	//7
	IIC_Stop();  	// Create a stop condition  	8
	HAL_Delay(10);
}

void IIC_Read_One_Byte(uint8_t daddr, uint8_t addr, uint8_t *data) {
	IIC_Start();  //1

	IIC_Send_Byte(daddr);	   // Send write command 	2
	IIC_Wait_Ack();	   //3
	IIC_Send_Byte(addr);	   // Sending address 	4
	IIC_Wait_Ack();		 //5
	IIC_Start();  	 	//6
	IIC_Send_Byte(daddr | 0X01);  	 	// Enter receive mode 		7
	IIC_Wait_Ack();	// 8
	*data = IIC_Read_Byte(0);		   //9 10
	IIC_Stop();		   // Create a stop condition 	    //11
}

void IIC_WriteBytes(uint8_t WriteAddr, uint8_t *data, uint8_t dataLength) {
	uint8_t i;
	IIC_Start();

	IIC_Send_Byte(WriteAddr);	    // Send write command
	IIC_Wait_Ack();

	for (i = 0; i < dataLength; i++) {
		IIC_Send_Byte(data[i]);
		IIC_Wait_Ack();
	}
	IIC_Stop();	    // Create a stop condition
	HAL_Delay(10);
}

void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr, uint8_t *data,
		uint8_t dataLength) {
	uint8_t i;
	IIC_Start();
	IIC_Send_Byte(deviceAddr);	    // Send write command
	IIC_Wait_Ack();
	IIC_Send_Byte(writeAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(deviceAddr | 0X01);	    // Enter receive mode
	IIC_Wait_Ack();

	for (i = 0; i < dataLength - 1; i++) {
		data[i] = IIC_Read_Byte(1);
	}
	data[dataLength - 1] = IIC_Read_Byte(0);
	IIC_Stop();	    // Create a stop condition
	HAL_Delay(10);
}

