/*
 *	bootloader.c
 *	Created on: May 31, 2025
 *		Author: Trần Minh Đức
 */

#include "bootloader.h"
#include "gpio_config.h"
#include "retarget.h"
#include "common.h"
#include "hld_flash.h"
#include "hld_uart.h"
#include "ymodem.h"
#include "crc32.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

uint8_t aFileName[FILE_NAME_LENGTH];

FirmwareHeader_t *pHeader;

uint32_t fw_type, fw_size, fw_version, fw_checksum_value;

void Bootloader_Task(void)
{
	Bootloader_YmodemReceive();

	Bootloader_Get_InforFW();

	Bootloader_JumpToApplication();
}

eBootloader_Status Bootloader_JumpToApplication(void)
{
	eBootloader_Status result = BOOT_STATUS_ERROR;
	uint8_t emptyCellCount = 0;

	for (uint8_t i = 0; i < 10; i++)
	{
		if (readWord(APP_START_ADDR + (i * 4)) == -1)
		{
			emptyCellCount++;
		}
	}

	if (emptyCellCount != 10)
	{
		result = BOOT_STATUS_OK;
	}

	if(result == BOOT_STATUS_OK)
	{
		eApp_Selection Select_App = Bootloader_SelectApp();

		switch(Select_App)
		{
		case APP_1:
		{
			if(Verify_CRC((uint8_t *)APP_START_ADDR, fw_size, fw_checksum_value) == BOOT_STATUS_OK)
			{
				printf("\r\nAPP1 is Called\r\n");
				jumpToApp(APP_START_ADDR);
			}
			else
			{
				printf("\r\nCheck CRC Fail");
			}
			break;
		}

		case APP_2:
		{
			printf("\r\nAPP2 is Called\r\n");
			jumpToApp(APP_START_ADDR);
			break;
		}

		default:
			break;
		}
	}

	return result;
}

void Bootloader_YmodemReceive(void)
{
	uint32_t size = 0;
	COM_StatusTypeDef result;

	Serial_PutString((uint8_t *)"\r\n[BOOT] Waiting for firmware transfer via YMODEM...\r\n");
	Serial_PutString((uint8_t *)"Hint: Use TeraTerm or another YMODEM tool on PC to send the .bin file\n\r");
	Serial_PutString((uint8_t *)"Press 'a' to cancel.\r\n");

	result = Ymodem_Receive(&size);

	if (result == COM_OK)
	{
		printf("\r\n======> File received and written successfully! <======\r\n");
		printf("\r\nFile name: %s", (char *)aFileName);
	}
	else if (result == COM_LIMIT)
	{
		printf("\r\nFile size exceeds allowed flash region!\r\n");
	}
	else if (result == COM_DATA)
	{
		printf("\r\nError writing to flash!\r\n");
	}
	else if (result == COM_ABORT)
	{
		printf("\r\nTransfer aborted by user.\r\n");
	}
	else
	{
		printf("\r\nUnknown error occurred during file transfer.\r\n");
	}
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

eBootloader_Status Verify_CRC(uint8_t *address, uint32_t length, uint32_t crc_expected)
{
	eBootloader_Status result = BOOT_STATUS_OK;
	uint32_t calc_crc = CRC32_CalculateBuffer(address, length);

	printf("\r\nExpected CRC: 0x%08lX", crc_expected);
	printf("\r\nCalculated CRC: 0x%08lX", calc_crc);

	if(calc_crc != crc_expected)
	{
		result = BOOT_STATUS_CRC_FAIL;
	}

	return result;
}

void Bootloader_Get_InforFW(void)
{
	pHeader = (FirmwareHeader_t *)FW_HEADER_ADDR;

	fw_type = pHeader->firmwareType;
	fw_size = pHeader->firmwareSize;
	fw_version = pHeader->firmwareVersion;
	fw_checksum_value = pHeader->checksumValue;

	fw_ver.bMajor = (uint8_t)(fw_version >> 24);
	fw_ver.bMinor = (uint8_t)(fw_version >> 16);
	fw_ver.Sub_minor = (uint16_t)(fw_version);

	printf("\r\nFirmware Version: %d.%d.%d", fw_ver.bMajor, fw_ver.bMinor, fw_ver.Sub_minor);
	printf("\r\nSize: %lu", fw_size);
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
