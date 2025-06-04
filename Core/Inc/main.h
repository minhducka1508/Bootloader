/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_def.h"

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

void SystemClock_Config(void);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
