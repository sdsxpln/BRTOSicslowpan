/**
 * \file rs485.c
 * \brief RS485 functions
 *
 * This file implements the rs485/uart driver.
 *
 **/

#include "hardware.h"
#include "uart.h"
#include "rs485.h"

#pragma warn_implicitconv off

#define RS485_BUFSIZE	16


#if UART_RS485 == UART1
#define RS485_PUTCHAR(x) 	putchar_uart1(x)
#define RS485_PRINTF(x)		printf_uart1(x)
#define RS485_RX_ENABLE()	uart1_RxEnable();uart1_RxEnableISR();
#define RS485_RX_DISABLE()	uart1_RxDisable();uart1_RxDisableISR();
extern OS_QUEUE SerialPortBuffer1;
extern BRTOS_Queue *Serial1;
#define RS485_QUEUE Serial1	
#elif UART_RS485 == UART2
#define RS485_PUTCHAR(x) 	putchar_uart2(x)
#define RS485_PRINTF(x)		printf_uart2(x)
#define RS485_RX_ENABLE()	uart2_RxEnableISR()
#define RS485_RX_DISABLE()	uart2_RxDisableISR()
extern OS_QUEUE SerialPortBuffer2;
extern BRTOS_Queue *Serial2;
#define RS485_QUEUE Serial2	
#endif

#define RS485_TX 	1
#define RS485_RX 	0
#define RS485_TXRX_PIN 		PTFD_PTFD2
#define RS485_TXRX_PINDIR 	PTFDD_PTFDD2

void rs485_init(INT16U baudrate, INT8U mutex, INT8U priority)
{
	
	RS485_TXRX_PIN = RS485_RX;
	RS485_TXRX_PINDIR = 1;
	
#if UART_RS485 == UART1
	uart_init(1,baudrate,RS485_BUFSIZE,mutex,priority);
#elif UART_RS485 == UART2	
	uart_init(2,baudrate,RS485_BUFSIZE,mutex,priority);
#endif	
}

void putchar_rs485(byte caracter)
{
	RS485_TXRX_PIN = RS485_TX;
	RS485_RX_DISABLE();
	RS485_PUTCHAR(caracter);
	RS485_RX_ENABLE();
	RS485_TXRX_PIN = RS485_RX;
}

void printf_rs485(CHAR8 *string)
{
	RS485_TXRX_PIN = RS485_TX;
	RS485_RX_DISABLE();
	RS485_PRINTF(string);
	RS485_RX_ENABLE();
	RS485_TXRX_PIN = RS485_RX;
}

void tx_rs485(INT8U *data, INT16U len)
{
	RS485_TXRX_PIN = RS485_TX;
	RS485_RX_DISABLE();
	while(len > 0)
	{
		RS485_PUTCHAR(data);
		data++;		
		len--;
	}
	RS485_RX_ENABLE();
	RS485_TXRX_PIN = RS485_RX;
}

void rx_rs485(CHAR8* caracter)
{	
	RS485_TXRX_PIN = RS485_RX;
	OSQueuePend(RS485_QUEUE, caracter, 0);	
	RS485_TXRX_PIN = RS485_RX;
}
