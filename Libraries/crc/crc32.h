
#ifndef _CRC32_H_
#define _CRC32_H_

#include <main.h>

#define CRC32_ENABLE_REFLECT_BIT_ORDER      (1)
/* CRC32_POLY */
#define CRC32_POLY                          (0x04C11DB7)

void CRC32_Start(void);

uint32_t CRC32_Accumulate(const uint8_t * buffer, uint32_t length);

uint32_t CRC32_Get(void);

uint32_t CRC32_CalculateBuffer(const uint8_t * buffer, uint32_t length);

#endif /* _CRC32_H_ */
