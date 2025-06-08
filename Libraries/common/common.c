/*
 *	common.c
 *	Created on: Jun 4, 2025
 *	    Author: Trần Minh Đức
 */

#include "common.h"
#include "hld_uart.h"

void Int2Str(uint8_t *p_str, uint32_t intnum)
{
	uint32_t i, divider = 1000000000, pos = 0;
	uint8_t started = 0;

	for (i = 0; i < 10; i++)
	{
		uint8_t digit = intnum / divider;
		intnum %= divider;
		divider /= 10;

		if (digit != 0 || started || i == 9) // giữ số 0 cuối cùng nếu toàn là 0
		{
			p_str[pos++] = digit + '0';
			started = 1;
		}
	}

	p_str[pos] = '\0';
}

uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if ((p_inputstr[0] == '0') && ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X')))
  {
    i = 2;
    while ( ( i < 11 ) && ( p_inputstr[i] != '\0' ) )
    {
      if (ISVALIDHEX(p_inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(p_inputstr[i]);
      }
      else
      {
        res = 0;
        break;
      }
      i++;
    }

    if (p_inputstr[i] == '\0')
    {
      *p_intnum = val;
      res = 1;
    }
  }
  else
  {
    while ( ( i < 11 ) && ( res != 1 ) )
    {
      if (p_inputstr[i] == '\0')
      {
        *p_intnum = val;
        res = 1;
      }
      else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K')) && (i > 0))
      {
        val = val << 10;
        *p_intnum = val;
        res = 1;
      }
      else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M')) && (i > 0))
      {
        val = val << 20;
        *p_intnum = val;
        res = 1;
      }
      else if (ISVALIDDEC(p_inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(p_inputstr[i]);
      }
      else
      {
        res = 0;
        break;
      }
      i++;
    }
  }

  return res;
}

void Serial_PutString(uint8_t *p_string)
{
  uint16_t length = 0;

  while (p_string[length] != '\0')
  {
    length++;
  }
  HLD_UART_Transmit(&UART_BOOTLOADER, p_string, length, TX_TIMEOUT);
}

HAL_StatusTypeDef Serial_PutByte( uint8_t param )
{
  return HLD_UART_Transmit(&UART_BOOTLOADER, &param, 1, TX_TIMEOUT);
}
