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

#include "uart_config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"
#include <stdbool.h>
#include <stdint.h>

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
