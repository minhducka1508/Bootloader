/*
 *	hld_flash.c
 *	Created on: Jun 4, 2025
 *		  Author: Trần Minh Đức
 */

#ifndef _HLD_FLASH_H_
#define _HLD_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

enum 
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_PROTECTION_ERRROR
};

enum {
  FLASHIF_PROTECTION_NONE         = 0x00,
  FLASHIF_PROTECTION_WRPENABLED   = 0x01
};

enum {
  FLASHIF_WRP_ENABLE,
  FLASHIF_WRP_DISABLE
};

#define APP_START_ADDR 0x08008000
//#define APP2_START_ADDR 0x08008000

#define APP_FLASH_SIZE ((uint32_t)(224 * 1024))

#define APP_END_ADDR ((uint32_t)(APP_START_ADDR + APP_FLASH_SIZE - 1))

/* Bitmap of sectors to protect (example: sectors 2–7 = bits 2 to 7) */
#define FLASH_SECTORS_TO_BE_PROTECTED  ((uint32_t)(0x000000FC))  // sectors 2 to 7

void     FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t start_address);
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length);
uint32_t FLASH_If_GetWriteProtectionStatus(void);
uint32_t FLASH_If_WriteProtectionConfig(uint32_t protectionstate);
uint32_t GetSector(uint32_t Address);

#ifdef __cplusplus
}
#endif

#endif /* _HLD_FLASH_H_ */
