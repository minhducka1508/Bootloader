/**
  ******************************************************************************
  * @file    hld_flash.h
  * @brief   This file provides all the headers of the flash_if functions,
  *          adapted for STM32F4 (sector-based flash layout).
  ******************************************************************************
  */

#ifndef _HLD_FLASH_H_
#define _HLD_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported constants --------------------------------------------------------*/

/* Error code */
enum 
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_PROTECTION_ERRROR
};

/* Protection types */
enum {
  FLASHIF_PROTECTION_NONE         = 0x00,
  FLASHIF_PROTECTION_WRPENABLED   = 0x01
};

/* Write protection config states */
enum {
  FLASHIF_WRP_ENABLE,
  FLASHIF_WRP_DISABLE
};

/* Flash layout for STM32F411RE (512KB, sectors 0–7)
 *   - Sector 0: 16 KB  @ 0x08000000
 *   - Sector 1: 16 KB  @ 0x08004000
 *   - Sector 2: 16 KB  @ 0x08008000
 *   - Sector 3: 16 KB  @ 0x0800C000
 *   - Sector 4: 64 KB  @ 0x08010000
 *   - Sector 5: 128 KB @ 0x08020000
 *   - Sector 6: 128 KB @ 0x08040000
 *   - Sector 7: 128 KB @ 0x08060000
 */

/* Define the start address where user app will be loaded (must align with a sector boundary) */
#define APP_START_ADDR 0x08008000
//#define APP2_START_ADDR 0x08008000

#define APP_FLASH_SIZE ((uint32_t)(224 * 1024))

#define APP_END_ADDR ((uint32_t)(APP_START_ADDR + APP_FLASH_SIZE - 1))

/* Bitmap of sectors to protect (example: sectors 2–7 = bits 2 to 7) */
#define FLASH_SECTORS_TO_BE_PROTECTED  ((uint32_t)(0x000000FC))  // sectors 2 to 7

/* Exported functions --------------------------------------------------------*/
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
