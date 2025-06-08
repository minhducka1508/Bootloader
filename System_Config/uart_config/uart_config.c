/*
 *	uart_config.c
 *	Created on: May 15, 2025
 *		Author: Trần Minh Đức
 */

#include "uart_config.h"

/* Config UART1 ------------------------------------------------------------*/
UART_HandleTypeDef huart1;

HLD_UART_Runtime_t uart1_runtime = {0};

HLD_UART_Config_t uart1_config = 
{
	.huart = &huart1,
	.tx_mode = HLD_UART_MODE_POLLING,
	.rx_mode = HLD_UART_MODE_POLLING,
	.tx_cb = NULL,
	.rx_cb = NULL
};

HLD_UART_t uart1_handle =
{
	.config = &uart1_config,
	.runtime = &uart1_runtime
};

void UART1_Config_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}

/* Config UART6 ------------------------------------------------------------*/
UART_HandleTypeDef huart6;

HLD_UART_Runtime_t uart6_runtime = {0};

HLD_UART_Config_t uart6_config =
{
	.huart = &huart6,
	.tx_mode = HLD_UART_MODE_POLLING,
	.rx_mode = HLD_UART_MODE_POLLING,
	.tx_cb = NULL,
	.rx_cb = NULL
};

HLD_UART_t uart6_handle =
{
	.config = &uart6_config,
	.runtime = &uart6_runtime
};

void UART6_Config_Init(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE(); // Dùng PC6 (TX), PC7 (RX)
	__HAL_RCC_USART6_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// PC6 -> TX, PC7 -> RX cho USART6
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	huart6.Instance = USART6;
	huart6.Init.BaudRate = 115200;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart6);

	HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART6_IRQn);
}

void USART6_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart6);
}
