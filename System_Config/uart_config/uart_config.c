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

/* Config UART2 ------------------------------------------------------------*/
UART_HandleTypeDef huart2;

HLD_UART_Runtime_t uart2_runtime = {0};

HLD_UART_Config_t uart2_config = 
{
	.huart = &huart2,
	.tx_mode = HLD_UART_MODE_POLLING,
	.rx_mode = HLD_UART_MODE_POLLING,
	.tx_cb = NULL,
	.rx_cb = NULL
};

HLD_UART_t uart2_handle =
{
	.config = &uart2_config,
	.runtime = &uart2_runtime
};

void UART2_Config_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();     // Nếu đã enable rồi thì không cần gọi lại
	__HAL_RCC_USART2_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// PA2 -> TX, PA3 -> RX cho USART2
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
}
