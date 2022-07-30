/*
 * IIC.h
 *
 *  Created on: Jul 24, 2022
 *      Author: emisario
 */

#ifndef INC_IIC_H_
#define INC_IIC_H_

#include "main.h"
#include "IIC.h"

#define SCL_PIN		GPIO_PIN_9
#define SCL_PORT	GPIOA

#define SDA_PIN 	GPIO_PIN_7
#define SDA_PORT	GPIOC

#define SDA_IN()  GPIOC->MODER &= ~((1<<14) | (1<<15))
#define SDA_OUT() {GPIOC->MODER &= ~ (1<<15); GPIOC->MODER |= (1 << 14);}

void IIC_Init(void);                // initialization IIC Of IO mouth
void IIC_Start(void);				// send out IIC Start signal
void IIC_Stop(void);	  			// send out IIC Stop signal
void IIC_Send_Byte(uint8_t txd);			//IIC Send a byte
uint8_t IIC_Read_Byte(unsigned char ack);//IIC Read a byte
uint8_t IIC_Wait_Ack(void); 				//IIC wait for ACK The signal
void IIC_Ack(void);					//IIC send out ACK The signal
void IIC_NAck(void);				//IIC Do not send ACK The signal

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data);

void IIC_SCL(uint8_t val);
void IIC_SDA(uint8_t val);
uint8_t READ_SDA();


#endif /* INC_IIC_H_ */
