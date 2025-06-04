/*
 *	gpio_config.h
 *	Created on: June 2, 2025
 *		Author: Trần Minh Đức
 */

#ifndef _GPIO_CONFIG_H_
#define _GPIO_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define LED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOA

#define App_Pin GPIO_PIN_13
#define App_GPIO_Port GPIOC

void GPIO_Config_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*_GPIO_CONFIG_H_*/
