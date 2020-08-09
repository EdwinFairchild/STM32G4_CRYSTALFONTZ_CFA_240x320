#include <stm32g4xx_ll_bus.h>
#include <stm32g4xx_ll_gpio.h>
#include <stm32g4xx_ll_usart.h>
#include <stm32g4xx_ll_tim.h>
#include <stm32g4xx_ll_rcc.h>
#include <stm32g4xx_ll_system.h>
#include <stm32g4xx_ll_utils.h>
#include <stm32g4xx_ll_spi.h>
#include <stm32g4xx_ll_dma.h>
#include <stm32g4xx_ll_dmamux.h>
#include <stm32g4xx_ll_cortex.h>

//-------- COMM LIBS
#include "CL_CONFIG.h"
#include "CL_delay.h"
#include "CL_systemClockUpdate.h"
#include "CL_printMsg.h"
#include "CL_CFA_240x320.h"


//-------- Marcos  /  Defines / Vars
#define LED_A1 LL_GPIO_PIN_0	

//-------- Prototypes

void init_spi(void);
void spiSend(uint8_t *data, uint32_t len);
void led0Setup(void);
void tftGPIO_init(void);

/*
 * Pin Definitions / Maping to TFT and Touch
 * 
 * LCD_MOSI		24 -->	PA7 SPI MOSI
 * LCD_SCL		33  --> PA5 SPI CLK
 * LCD_ WRB		32 -->
 * LCD_CS		34 --> 
 * LCD_RESET	35 -->
 * 
 * Touch_GND     1/12
 * Touch_Reset	2/11 -->
 * Touch_IRQ	3/10 -->
 * Touch_SDA	4/9  --> PA8 I2C SDA
 * Touch_SCL	5/8  --> PA9 I2C CLK
 * Touch_V+     6/7
 * 
 *
 **/

int main(void)
{
	CL_setSysClockTo170();
	CL_printMsg_init_Default(false);
	CL_delay_init();
	led0Setup();
	CL_printMsg("\nCL init success\n");																																																																																																																																																																																																						
	
	
	init_spi();
	
	//-------| TODO: 
	//	configure pins for TFT and touch
	tftGPIO_init();
		
	//--------
	


	for (;;)
	{

	}
}

void init_spi(void)
{
	//clocks	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	
	
	LL_GPIO_InitTypeDef myGPIO;
	LL_GPIO_StructInit(&myGPIO);
	myGPIO.Pin       = LL_GPIO_PIN_5 | LL_GPIO_PIN_7;
	myGPIO.Speed     = LL_GPIO_SPEED_FREQ_HIGH;
	myGPIO.Mode      = LL_GPIO_MODE_ALTERNATE;
	myGPIO.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(GPIOA, &myGPIO);
	
	LL_SPI_InitTypeDef mySPI;
	LL_SPI_StructInit(&mySPI);
	mySPI.Mode				= LL_SPI_MODE_MASTER;
	mySPI.TransferDirection	= LL_SPI_HALF_DUPLEX_TX;
	mySPI.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV16;
	mySPI.NSS				= LL_SPI_NSS_SOFT;
	mySPI.DataWidth			= LL_SPI_DATAWIDTH_8BIT;
	
	LL_SPI_Init(SPI1, &mySPI);
	LL_SPI_EnableDMAReq_TX(SPI1);
	LL_SPI_Enable(SPI1);
	
}
void SPI_sendCommand(uint8_t cmd)
{
	// Select the LCD controller

	CLR_CS;
	//Send the command via SPI:
	CLR_RS;
	SPI.transfer(cmd);
	// Deselect the LCD controller
	SET_CS;
}
//==============================================================================
void SPI_sendData(uint8_t data)
{
	// Select the LCD controller
	CLR_CS;
	//Send the data via SPI:
	SET_RS;
	SPI.transfer(data);
	// Deselect the LCD controller
	SET_CS;
}
void spiSend(uint8_t *data, uint32_t len)
{
	
	uint8_t volatile *spidr = ((__IO uint8_t *)&SPI1->DR);
	while (len > 0)
	{
		while (!(SPI1->SR&SPI_SR_TXE)) {
			;
		}
		*spidr = *data++;
		len--;
	}	
	while ((SPI1->SR&SPI_SR_BSY)) {
		;
	}
	
	
}
void led0Setup(void)
{
 
	
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	
	LL_GPIO_InitTypeDef myGPIO;
	LL_GPIO_StructInit(&myGPIO);
	myGPIO.Mode			= LL_GPIO_MODE_OUTPUT;
	myGPIO.OutputType	= LL_GPIO_OUTPUT_PUSHPULL;
	myGPIO.Pin			= LED_A1;
	myGPIO.Speed		= LL_GPIO_SPEED_HIGH;
	LL_GPIO_Init(GPIOA, &myGPIO);
}
void tftGPIO_init(void)
{
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7);
	LL_GPIO_InitTypeDef myGPIO;
	LL_GPIO_StructInit(&myGPIO);
	myGPIO.Mode			= LL_GPIO_MODE_OUTPUT;
	myGPIO.OutputType	= LL_GPIO_OUTPUT_PUSHPULL;
	myGPIO.Pin			=  LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
	myGPIO.Speed		= LL_GPIO_SPEED_LOW;
	LL_GPIO_Init(GPIOB, &myGPIO);
	

}