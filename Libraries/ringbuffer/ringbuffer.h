/*
 *   ringbuffer.h
 *   Created on: May 10, 2025
 *       Author: Trần Minh Đức
 */

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"

#define 	ATCOMMAND_MAX_ARGUMENT		(10)

typedef enum
{
    eResult_Error = 0,
    eResult_OK
} eRingBuffer_Result_t;

typedef enum
{
    eRingBuffer_Type8bit = 0,
    eRingBuffer_Type32bit
} eRingBuffer_Type_t;

typedef struct
{
    uint8_t *bufferData8;
    uint32_t *bufferData32;
    uint16_t totalSize;
    uint16_t writeIndex;
    uint16_t readIndex;
    eRingBuffer_Type_t type;
} sRingBuffer_Handel_t;

#ifdef __cplusplus
}
#endif

#endif /* _RINGBUFFER_H_ */