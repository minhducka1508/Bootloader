/*
 *	bootloader.h
 *	Created on: May 31, 2025
 *		Author: Trần Minh Đức
 */

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "uart_config.h"

#define UART_BOOTLOADER uart1_handle
#define UART_DEBUG      uart6_handle

typedef enum
{
    BOOT_STATUS_OK = 0U,
    BOOT_STATUS_ERROR,
    BOOT_STATUS_INVALID_CMD,
    BOOT_STATUS_CRC_FAIL,
    BOOT_STATUS_FLASH_FAIL,
    BOOT_STATUS_VERIFICATION_FAIL,
    BOOT_STATUS_TIMEOUT,
} eBootloader_Status;

typedef enum
{
	JUMP_MODE,
	FLASH_MODE,
} eBootloader_Mode;

typedef enum
{
	APP_1,
	APP_2
} eApp_Selection;

typedef void(application_t)(void);

typedef struct
{
	uint32_t stack_addr;   // Stack Pointer
	application_t *func_p; // Program Counter
} JumpStruct;

/* Functions -----------------------------------------------------------------*/
void Bootloader_Init(void);
void Bootloader_Task(void);
eBootloader_Status Bootloader_JumpToApplication(void);
void Bootloader_YmodemReceive(void);
eApp_Selection Bootloader_SelectApp(void);

uint32_t readWord(uint32_t address);

void jumpToApp(const uint32_t address);

void deinitEverything();

uint8_t Bootloader_Erase(void);

uint8_t Bootloader_FlashBegin(void);
uint8_t Bootloader_FlashNext(uint64_t data);
uint8_t Bootloader_FlashEnd(void);

uint8_t Bootloader_GetProtectionStatus(void);
uint8_t Bootloader_ConfigProtection(uint32_t protection);

uint8_t Bootloader_CheckSize(uint32_t appsize);
uint8_t Bootloader_VerifyChecksum(void);
uint8_t Bootloader_CheckForApplication(void);

uint32_t Bootloader_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* _BOOTLOADER_H_ */
