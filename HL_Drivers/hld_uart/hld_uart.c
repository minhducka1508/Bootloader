/*
 *   uart.c
 *   Created on: May 11, 2025
 *       Author: Trần Minh Đức
 */

#include "hld_uart.h"
#include "stm32f4xx_hal_def.h"

HLD_UART_Status_t HLD_UART_Init(const HLD_UART_t *uart)
{
	if (uart == NULL || uart->config == NULL || uart->config->huart == NULL)
	{
		return HLD_UART_ERROR;
	}

	uart->runtime->tx_busy = false;
	uart->runtime->rx_busy = false;

	return HLD_UART_OK;
}

HLD_UART_Status_t HLD_UART_Transmit(const HLD_UART_t *uart, const uint8_t *data, uint32_t data_length)
{
	if (uart == NULL || uart->runtime->tx_busy)
	{
		return HLD_UART_BUSY;
	}

	uart->runtime->tx_busy = true;
	HAL_StatusTypeDef result = HAL_UART_Transmit(uart->config->huart, (uint8_t *)data, data_length, HAL_MAX_DELAY);
	uart->runtime->tx_busy = false;

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}

HLD_UART_Status_t HLD_UART_Receive(const HLD_UART_t *uart, uint8_t *data, uint32_t data_length)
{
	if (uart == NULL || uart->runtime->rx_busy)
	{
		return HLD_UART_BUSY;
	}

	uart->runtime->rx_busy = true;
	HAL_StatusTypeDef result = HAL_UART_Receive(uart->config->huart, data, data_length, HAL_MAX_DELAY);
	uart->runtime->rx_busy = false;

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}

HLD_UART_Status_t HLD_UART_AbortTransmit(const HLD_UART_t *uart)
{
	if (uart == NULL)
	{
		return HLD_UART_ERROR;
	}

	HAL_UART_AbortTransmit(uart->config->huart);
	uart->runtime->tx_busy = false;

	return HLD_UART_OK;
}

HLD_UART_Status_t HLD_UART_AbortReceive(const HLD_UART_t *uart)
{
	if (uart == NULL)
	{
		return HLD_UART_ERROR;
	}

	HAL_UART_AbortReceive(uart->config->huart);
	uart->runtime->rx_busy = false;

	return HLD_UART_OK;
}

HLD_UART_Status_t HLD_UART_StartTransmitIRQ(const HLD_UART_t *uart, const uint8_t *data, uint32_t data_length)
{
	if (uart == NULL || uart->runtime->tx_busy)
	{
		return HLD_UART_BUSY;
	}

	uart->runtime->tx_busy = true;
	uart->runtime->tx_buf = (uint8_t *)data;
	uart->runtime->tx_len = data_length;
	HAL_StatusTypeDef result = HAL_UART_Transmit_IT(uart->config->huart, (uint8_t *)data, data_length);

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}

HLD_UART_Status_t HLD_UART_StartReceiveIRQ(const HLD_UART_t *uart, uint8_t *data, uint32_t data_length)
{
	if (uart == NULL || uart->runtime->rx_busy)
	{
		return HLD_UART_BUSY;
	}

	uart->runtime->rx_busy = true;
	uart->runtime->rx_buf = (uint8_t *)data;
	uart->runtime->rx_len = data_length;
	HAL_StatusTypeDef result = HAL_UART_Receive_IT(uart->config->huart, data, data_length);

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}

HLD_UART_Status_t HLD_UART_StartTransmitDMA(const HLD_UART_t *uart, const uint8_t *data, uint32_t data_length)
{
	if (uart == NULL || uart->runtime->tx_busy)
	{
		return HLD_UART_BUSY;
	}

	uart->runtime->tx_busy = true;
	uart->runtime->tx_buf = (uint8_t *)data;
	uart->runtime->tx_len = data_length;
	HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(uart->config->huart, (uint8_t *)data, data_length);

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}

HLD_UART_Status_t HLD_UART_StartReceiveDMA(const HLD_UART_t *uart, uint8_t *data, uint32_t data_length)
{
	if (uart == NULL || uart->runtime->tx_busy)
	{
		return HLD_UART_BUSY;
	}

	uart->runtime->rx_busy = true;
	uart->runtime->rx_buf = (uint8_t *)data;
	uart->runtime->rx_len = data_length;
	HAL_StatusTypeDef result = HAL_UART_Receive_DMA(uart->config->huart, data, data_length);

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}

HLD_UART_Status_t HLD_UART_SetBaudrate(const HLD_UART_t *uart, uint32_t baud)
{
	if (uart == NULL)
	{
		return HLD_UART_ERROR;
	}

	uart->config->huart->Init.BaudRate = baud;
	HAL_StatusTypeDef result = HAL_UART_Init(uart->config->huart);

	return (result == HAL_OK) ? HLD_UART_OK : HLD_UART_ERROR;
}
