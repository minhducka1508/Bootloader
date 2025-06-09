/*
 *	hld_flash.c
 *	Created on: Jun 4, 2025
 *		Author: Trần Minh Đức
 */

#include "hld_flash.h"

#define FLASHIF_OK 0
#define FLASHIF_ERASEKO 1
#define FLASHIF_WRITING_ERROR 2
#define FLASHIF_WRITINGCTRL_ERROR 3
#define FLASHIF_PROTECTION_ERRROR 4
#define FLASHIF_PROTECTION_NONE 0
#define FLASHIF_PROTECTION_WRPENABLED 1

/**
 * @brief  Unlocks Flash for write access
 */
void FLASH_If_Init(void)
{
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	HAL_FLASH_Lock();
}

/**
 * @brief  Erases the flash from start address to end address
 * @param  start: start address to erase from
 */
uint32_t FLASH_If_Erase(uint32_t start)
{
	HAL_StatusTypeDef status;
	uint32_t FirstSector, NbOfSectors, SectorError;
	FLASH_EraseInitTypeDef EraseInitStruct;

	HAL_FLASH_Unlock();

	FirstSector = GetSector(start);
	NbOfSectors = GetSector(APP_END_ADDR) - FirstSector + 1;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FirstSector;
	EraseInitStruct.NbSectors = NbOfSectors;

	status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

	HAL_FLASH_Lock();

	return (status == HAL_OK) ? FLASHIF_OK : FLASHIF_ERASEKO;
}

/**
 * @brief  Writes data to flash
 */
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length)
{
	uint32_t i;

	HAL_FLASH_Unlock();

	for (i = 0; i < length; i++)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination, p_source[i]) == HAL_OK)
		{
			if (*(uint32_t *)destination != p_source[i])
			{
				HAL_FLASH_Lock();
				return FLASHIF_WRITINGCTRL_ERROR;
			}
			destination += 4;
		}
		else
		{
			HAL_FLASH_Lock();
			return FLASHIF_WRITING_ERROR;
		}
	}

	HAL_FLASH_Lock();
	return FLASHIF_OK;
}

/**
 * @brief  Returns the write protection status.
 */
uint32_t FLASH_If_GetWriteProtectionStatus(void)
{
	FLASH_OBProgramInitTypeDef OBInit;
	uint32_t ProtectedSectors = FLASHIF_PROTECTION_NONE;

	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();

	HAL_FLASHEx_OBGetConfig(&OBInit);

	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();

	if ((~OBInit.WRPSector) & FLASH_SECTORS_TO_BE_PROTECTED)
	{
		ProtectedSectors = FLASHIF_PROTECTION_WRPENABLED;
	}

	return ProtectedSectors;
}

/**
 * @brief  Configures write protection
 */
uint32_t FLASH_If_WriteProtectionConfig(uint32_t protectionstate)
{
	FLASH_OBProgramInitTypeDef OBInit;
	HAL_StatusTypeDef result;

	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();

	HAL_FLASHEx_OBGetConfig(&OBInit);

	OBInit.OptionType = OPTIONBYTE_WRP;
	OBInit.WRPSector = FLASH_SECTORS_TO_BE_PROTECTED;
	OBInit.WRPState = (protectionstate == FLASHIF_PROTECTION_WRPENABLED) ? OB_WRPSTATE_ENABLE : OB_WRPSTATE_DISABLE;

	result = HAL_FLASHEx_OBProgram(&OBInit);

	HAL_FLASH_OB_Launch(); // apply new OBs
	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();

	return (result == HAL_OK) ? FLASHIF_OK : FLASHIF_PROTECTION_ERRROR;
}

/**
 * @brief  Maps address to flash sector
 */
uint32_t GetSector(uint32_t Address)
{
	if (Address < 0x08004000)
		return FLASH_SECTOR_0;
	else if (Address < 0x08008000)
		return FLASH_SECTOR_1;
	else if (Address < 0x0800C000)
		return FLASH_SECTOR_2;
	else if (Address < 0x08010000)
		return FLASH_SECTOR_3;
	else if (Address < 0x08020000)
		return FLASH_SECTOR_4;
	else if (Address < 0x08040000)
		return FLASH_SECTOR_5;
	else if (Address < 0x08060000)
		return FLASH_SECTOR_6;
	else
		return FLASH_SECTOR_7;
}
