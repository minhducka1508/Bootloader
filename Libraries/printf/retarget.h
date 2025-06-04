/*
 *	retarget.h
 *	Created on: June 2, 2025
 *		Author: Trần Minh Đức
 */

#ifndef _RETARGET_H_
#define _RETARGET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void RetargetInit(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif
#endif /* _RETARGET_H_ */
