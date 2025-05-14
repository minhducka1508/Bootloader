/*
*   ringbuffer.c
*   Created on: May 10, 2025
*       Author: Trần Minh Đức
*/

#include "ringbuffer.h"
#include "cmsis_gcc.h"
#include "stdbool.h"


eRingBuffer_Result_t ringBuffer_init(sRingBuffer_Handel_t *handle, eRingBuffer_Type_t ringbuffType, uint16_t numberOfElements)
{
	if(ringbuffType == eRingBuffer_Type8bit)
	{
		handle->bufferData8 = (uint8_t*)malloc(sizeof(uint8_t)  *(numberOfElements + 1));
		if(handle->bufferData8  == NULL) return eResult_Error;
	}
	else if (ringbuffType == eRingBuffer_Type32bit)
	{
		handle->bufferData32 = (uint32_t*)malloc(sizeof(uint32_t)  *(numberOfElements + 1));
		if(handle->bufferData32  == NULL) return eResult_Error;
	}
	handle->type = ringbuffType;

	handle->totalSize = numberOfElements + 1;
	handle->readIndex = 0;
	handle->writeIndex = 0;

	if(handle->type == eRingBuffer_Type8bit)
	{
		if(handle->bufferData8 != NULL)
		{
			return eResult_OK;
		}
		else
		{
			return eResult_Error;
		}
	}
	else if (handle->type == eRingBuffer_Type32bit)
	{
		if(handle->bufferData32 != NULL)
		{
			return eResult_OK;
		}
		else
		{
			return eResult_Error;
		}
	}
	else
	{
		return eResult_Error;
	}
}



eRingBuffer_Result_t ringBuffer_write(sRingBuffer_Handel_t *handle, uint32_t writeData)
{
	if(handle->type == eRingBuffer_Type8bit)
	{
		if(handle->bufferData8 == NULL) return eResult_Error;
	}
	else if (handle->type == eRingBuffer_Type32bit)
	{
		if(handle->bufferData32 == NULL) return eResult_Error;
	}

	__disable_irq();
	uint16_t next_index = (handle->writeIndex + 1) % (handle->totalSize);
	if(next_index != handle->readIndex)
	{
		if(handle->type == eRingBuffer_Type8bit)
		{
			*((uint8_t*)handle->bufferData8 + handle->writeIndex) = (uint8_t)writeData;
		}
		else if ( handle->type == eRingBuffer_Type32bit)
		{
			*((uint32_t*)handle->bufferData32 + handle->writeIndex) = (uint32_t)writeData;
		}
		else
		{
			__enable_irq();
			return eResult_Error;
		}

		handle->writeIndex = next_index;
		__enable_irq();
		return eResult_OK;
	}
	else
	{
		__enable_irq();
		return eResult_Error;
	}
}



eRingBuffer_Result_t ringBuffer_read(sRingBuffer_Handel_t *handle, uint32_t *readData)
{
	if(handle->type == eRingBuffer_Type8bit)
	{
		if(handle->bufferData8 == NULL) return eResult_Error;
	}
	else if (handle->type == eRingBuffer_Type32bit)
	{
		if(handle->bufferData32 == NULL) return eResult_Error;
	}
	__disable_irq();
	if(handle->writeIndex != handle->readIndex)
	{
		if(handle->type == eRingBuffer_Type8bit)
		{
			*readData = *(handle->bufferData8 + handle->readIndex);
		}
		else if(handle->type == eRingBuffer_Type32bit)
		{
			*readData = *(handle->bufferData32 + handle->readIndex);
		}
		else
		{
			__enable_irq();
			return eResult_Error;
		}
		handle->readIndex = (handle->readIndex + 1) % (handle->totalSize);
		__enable_irq();
		return eResult_OK;
	}
	else
	{
		__enable_irq();
		return eResult_Error;
	}
}


eRingBuffer_Result_t ringBuffer_flush(sRingBuffer_Handel_t *handle)
{
	__disable_irq();
	handle->readIndex = 0;
	handle->writeIndex = 0;
	__enable_irq();
	return eResult_OK;
}


void atcommand_split_param(uint8_t *inputData, uint8_t *argCount, uint8_t **argData)
{
	uint8_t* 	readp;
	uint8_t    	inside_token = 0;

	if((inputData != NULL) && (argCount != NULL) && (argData != NULL))
	{
		*argCount = 0;
		for (readp = inputData; *readp; readp++)
		{
			if (!inside_token)
			{
				if ((*readp == '=') || (*readp == ':') || (*readp == ',') || (*readp == '\r') || (*readp == '\n'))
				{
					/* :,\r\n is not copied */
				}
				else
				{
					/* Start of token */
					inside_token = 1;
					argData[*argCount] = readp;
					(*argCount)++;

					if((*argCount) == ATCOMMAND_MAX_ARGUMENT)
					{
						return;
					}
				}
			}
			else
			{
				/* inside token */
				if ((*readp == '=') || (*readp == ':') || (*readp == ',') || (*readp == '\r') || (*readp == '\n') || (*readp == '.'))
				{
					/* End of token */
					inside_token = 0;

					/* Assign null for terminate token */
					*readp =  0;
				}
				else
				{
					/* Continuation of token */
				}
			}
		}

		if (inside_token)
		{
			/* End of input line terminates a token */
			*readp = 0;
			readp++;
		}

		/* Null-terminate just to be nice */
		argData[*argCount] = 0;
	}
}
