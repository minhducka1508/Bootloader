/*
 *	uart_config.h
 *	Created on: May 15, 2025
 *		Author: Trần Minh Đức
 */

#ifndef _UART_CONFIG_H_
#define _UART_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"

	void UART1_Config(void);
	void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _UART_CONFIG_H_ */
