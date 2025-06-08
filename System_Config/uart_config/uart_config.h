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

#include "ringbuffer.h"
#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	HLD_UART_OK = 0x00U,
	HLD_UART_ERROR = 0x01U,
	HLD_UART_BUSY = 0x02U,
	HLD_UART_TIMEOUT = 0x03U
} HLD_UART_Status_t;

typedef enum
{
	HLD_UART_MODE_POLLING = 0,
	HLD_UART_MODE_INTERRUPT,
	HLD_UART_MODE_DMA
} HLD_UART_Mode_t;

typedef struct
{
	UART_HandleTypeDef *huart;
	HLD_UART_Mode_t tx_mode;
	HLD_UART_Mode_t rx_mode;
	void (*tx_cb)(void);
	void (*rx_cb)(uint8_t);
} HLD_UART_Config_t;

typedef struct
{
	uint8_t *tx_buf;
	uint32_t tx_len;
	volatile uint32_t tx_pos;
	volatile bool tx_busy;
	sRingBuffer_Handel_t tx_ringBuff;

	uint8_t *rx_buf;
	uint32_t rx_len;
	volatile uint32_t rx_pos;
	volatile bool rx_busy;
	sRingBuffer_Handel_t rx_ringBuff;
} HLD_UART_Runtime_t;

typedef struct
{
	HLD_UART_Config_t *config;
	HLD_UART_Runtime_t *runtime;
} HLD_UART_t;

extern HLD_UART_t uart1_handle;
extern HLD_UART_t uart6_handle;

void UART1_Config_Init(void);
void UART6_Config_Init(void);

void USART1_IRQHandler(void);
void USART6_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _UART_CONFIG_H_ */
