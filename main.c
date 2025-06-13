/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @author         : Trần Minh Đức
  * @date           : 15/05/2025
  *
  * @note
  *   - Dự án: Bootloader bảo mật
  *   - MCU  : STM32F411RE
  *
  ******************************************************************************
  * @copyright
  *
  * Bản quyền (c) 2025 bởi Trần Minh Đức. Tất cả các quyền được bảo lưu.
  *
  * Phần mềm này được cung cấp "nguyên trạng", không có bất kỳ bảo đảm nào.
  * Bạn có thể sử dụng, chỉnh sửa và phân phối lại tùy ý.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "hld_uart.h"
#include "retarget.h"
#include "gpio_config.h"
#include "bootloader.h"

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	GPIO_Config_Init();

	HLD_UART_Init(&uart1_handle);
	HLD_UART_Init(&uart6_handle);

	RetargetInit(uart6_handle.config->huart);

	Bootloader_Task();
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
