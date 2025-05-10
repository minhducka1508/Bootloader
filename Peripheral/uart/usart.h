#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

    extern UART_HandleTypeDef huart1;

    void MX_USART1_UART_Init(void);
    void UART_Transmit(uint8_t *data, uint32_t lenData);
    void UART_Receive(uint8_t *buf, uint32_t lenData);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
