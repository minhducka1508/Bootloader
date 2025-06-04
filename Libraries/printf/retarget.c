/*
 *	retarget.c
 *	Created on: June 2, 2025
 *		Author: Trần Minh Đức
 */

#include "retarget.h"
#include "uart_config.h"

UART_HandleTypeDef *gHuartPrintf = NULL;

void RetargetInit(UART_HandleTypeDef *huart)
{
    gHuartPrintf = huart;
}

int _write(int file, char *ptr, int len)
{
    if (gHuartPrintf != NULL)
        HAL_UART_Transmit(gHuartPrintf, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}
