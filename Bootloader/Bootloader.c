/*
 *	bootloader.c
 *	Created on: May 31, 2025
 *		Author: Trần Minh Đức
 */

#include "bootloader.h"
#include "gpio_config.h"
#include "retarget.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;

void Bootloader_Init(void)
{
	printf("====> Bootloader Init <====\r\n");

	Bootloader_Task();
}

eBootloader_Status Bootloader_Task(void)
{
	eBootloader_Status result = BOOT_STATUS_ERROR;
	uint8_t emptyCellCount = 0;

	for (uint8_t i = 0; i < 10; i++)
	{
		if (readWord(APP1_START_ADDR + (i * 4)) == -1)
		{
			emptyCellCount++;
		}
	}

	if (emptyCellCount != 10)
	{
		printf("BOOT_STATUS_OK\r\n");
		result = BOOT_STATUS_OK;
	}

	if(result == BOOT_STATUS_OK)
	{
		eApp_Selection Select_App = Bootloader_SelectApp();

		switch(Select_App)
		{
		case APP_1:
		{
			printf("APP1 is Called\r\n");
			jumpToApp(APP1_START_ADDR);
			break;
		}

		case APP_2:
		{
			printf("APP2 is Called\r\n");
			jumpToApp(APP2_START_ADDR);
			break;
		}

		default:
			break;
		}
	}

	return result;
}

eApp_Selection Bootloader_SelectApp(void)
{
	eApp_Selection result = APP_1;

	GPIO_PinState Select_App = HAL_GPIO_ReadPin(App_GPIO_Port, App_Pin);

	if(Select_App == GPIO_PIN_SET)
	{
		result = APP_1;
	}
	else
	{
		result = APP_2;
	}

	return result;
}

uint32_t readWord(uint32_t address)
{
	uint32_t read_data;
	read_data = *(uint32_t *)(address);
	return read_data;
}

void jumpToApp(uint32_t address)
{
    typedef void (*pFunction)(void);
    uint32_t appStack = *(volatile uint32_t *)address;
    uint32_t appResetHandler = *(volatile uint32_t *)(address + 4);
    pFunction appEntry = (pFunction)appResetHandler;

    deinitEverything();

    SCB->VTOR = address;
    __DSB();
    __ISB();

    __set_MSP(appStack);
    appEntry();
}

void deinitEverything()
{
    __disable_irq();  // Ngắt toàn bộ interrupt để tránh xung đột khi jump

	while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
	HAL_UART_DeInit(&huart1);  // Tắt UART nếu đang dùng printf()

    // Tắt clock cho các GPIO (tránh chiếm chân khi App dùng lại)
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    // Thêm các PORT khác nếu bạn đã bật trước đó

    // Tắt SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    HAL_RCC_DeInit();  // Đặt lại clock hệ thống về mặc định reset
    HAL_DeInit();      // Reset HAL

    SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk;  // Xóa pending interrupt nếu có
    __enable_irq();  // Có thể bật lại nếu App xử lý được interrupt
}
