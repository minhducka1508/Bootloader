/*
 *   hld_uart.h
 *   Created on: May 11, 2025
 *       Author: Trần Minh Đức
 */

#ifndef _HLD_UART_H_
#define _HLD_UART_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"
#include "ringbuffer.h"
#include <stdbool.h>
#include <stdint.h>

#define UART_COUNT (3)
	typedef enum
	{
		HLD_UART_OK = 0,
		HLD_UART_BUSY,
		HLD_UART_ERROR
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
	} HLD_UART_Runtime_t;

	typedef struct
	{
		HLD_UART_Config_t *config;
		HLD_UART_Runtime_t *runtime;
	} HLD_UART_t;

	// API
	HLD_UART_Status_t HLD_UART_Init(const HLD_UART_t *uart);
	HLD_UART_Status_t HLD_UART_Transmit(const HLD_UART_t *uart, const uint8_t *data, uint32_t len);
	HLD_UART_Status_t HLD_UART_Receive(const HLD_UART_t *uart, uint8_t *data, uint32_t len);
	HLD_UART_Status_t HLD_UART_AbortTransmit(const HLD_UART_t *uart);
	HLD_UART_Status_t HLD_UART_AbortReceive(const HLD_UART_t *uart);

	HLD_UART_Status_t HLD_UART_StartTransmitIRQ(const HLD_UART_t *uart, const uint8_t *data, uint32_t len);
	HLD_UART_Status_t HLD_UART_StartReceiveIRQ(const HLD_UART_t *uart, uint8_t *data, uint32_t len);
	HLD_UART_Status_t HLD_UART_StartTransmitDMA(const HLD_UART_t *uart, const uint8_t *data, uint32_t len);
	HLD_UART_Status_t HLD_UART_StartReceiveDMA(const HLD_UART_t *uart, uint8_t *data, uint32_t len);

	HLD_UART_Status_t HLD_UART_SetBaudrate(const HLD_UART_t *uart, uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif /* _HLD_UART_H_ */
