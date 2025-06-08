/*
 *	ymodem.c
 *	Created on: May 31, 2025
 *	    Author: Trần Minh Đức
 */

#include "hld_flash.h"
#include "hld_uart.h"
#include "bootloader.h"
#include "retarget.h"
#include "common.h"
#include "ymodem.h"
#include "aes.h"

#define CRC16_F /* activate the CRC16 integrity */

extern uint8_t aFileName[FILE_NAME_LENGTH];

uint8_t aPacketData[PACKET_1K_SIZE + PACKET_DATA_INDEX + PACKET_TRAILER_SIZE];

#define AES_KEY_LEN 16
#define AES_BLOCK_SIZE 16

static const uint8_t aes_key[AES_KEY_LEN] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
    0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81
};

static const uint8_t aes_iv[AES_BLOCK_SIZE] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f
};

/* Private function prototypes -----------------------------------------------*/
static HLD_UART_Status_t ReceivePacket(uint8_t *p_data, uint32_t *p_length, uint32_t timeout);
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte);
uint16_t Cal_CRC16(const uint8_t *p_data, uint32_t size);
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Receive a packet from sender
 * @param  data
 * @param  length
 *     0: end of transmission
 *     2: abort by sender
 *    >0: packet length
 * @param  timeout
 * @retval HAL_OK: normally return
 *         HAL_BUSY: abort by user
 */
static HLD_UART_Status_t ReceivePacket(uint8_t *p_data, uint32_t *p_length, uint32_t timeout)
{
	uint32_t crc;
	uint32_t packet_size = 0;
	HLD_UART_Status_t status;
	uint8_t char1;

	*p_length = 0;
	status = HLD_UART_Receive(&UART_BOOTLOADER, &char1, 1, timeout);

	if (status == HLD_UART_OK)
	{
		switch (char1)
		{
		case SOH:
			packet_size = PACKET_SIZE;
			break;
		case STX:
			packet_size = PACKET_1K_SIZE;
			break;
		case EOT:
			break;
		case CA:
			if ((HLD_UART_Receive(&UART_BOOTLOADER, &char1, 1, timeout) == HLD_UART_OK) && (char1 == CA))
			{
				packet_size = 2;
			}
			else
			{
				status = HLD_UART_ERROR;
			}
			break;
		case ABORT1:
		case ABORT2:
			status = HLD_UART_BUSY;
			break;
		default:
			status = HLD_UART_ERROR;
			break;
		}
		*p_data = char1;

		if (packet_size >= PACKET_SIZE)
		{
			status = HLD_UART_Receive(&UART_BOOTLOADER, &p_data[PACKET_NUMBER_INDEX], packet_size + PACKET_OVERHEAD_SIZE, timeout);

			/* Simple packet sanity check */
			if (status == HLD_UART_OK)
			{
				if (p_data[PACKET_NUMBER_INDEX] != ((p_data[PACKET_CNUMBER_INDEX]) ^ NEGATIVE_BYTE))
				{
					packet_size = 0;
					status = HLD_UART_ERROR;
				}
				else
				{
					/* Check packet CRC */
					crc = p_data[packet_size + PACKET_DATA_INDEX] << 8;
					crc += p_data[packet_size + PACKET_DATA_INDEX + 1];
					if (Cal_CRC16((uint8_t *)&p_data[PACKET_DATA_INDEX], packet_size) != crc)
					{
						packet_size = 0;
						status = HLD_UART_ERROR;
					}
				}
			}
			else
			{
				packet_size = 0;
			}
		}
	}
	*p_length = packet_size;
	return status;
}

/**
 * @brief  Update CRC16 for input byte
 * @param  crc_in input value
 * @param  input byte
 * @retval None
 */
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
	uint32_t crc = crc_in;
	uint32_t in = byte | 0x100;

	do
	{
		crc <<= 1;
		in <<= 1;
		if (in & 0x100)
			++crc;
		if (crc & 0x10000)
			crc ^= 0x1021;
	}

	while (!(in & 0x10000));

	return crc & 0xffffu;
}

/**
 * @brief  Cal CRC16 for YModem Packet
 * @param  data
 * @param  length
 * @retval None
 */
uint16_t Cal_CRC16(const uint8_t *p_data, uint32_t size)
{
	uint32_t crc = 0;
	const uint8_t *dataEnd = p_data + size;

	while (p_data < dataEnd)
		crc = UpdateCRC16(crc, *p_data++);

	crc = UpdateCRC16(crc, 0);
	crc = UpdateCRC16(crc, 0);

	return crc & 0xffffu;
}

/**
 * @brief  Calculate Check sum for YModem Packet
 * @param  p_data Pointer to input data
 * @param  size length of input data
 * @retval uint8_t checksum value
 */
uint8_t CalcChecksum(const uint8_t *p_data, uint32_t size)
{
	uint32_t sum = 0;
	const uint8_t *p_data_end = p_data + size;

	while (p_data < p_data_end)
	{
		sum += *p_data++;
	}

	return (sum & 0xffu);
}

/* Public functions ---------------------------------------------------------*/
/**
 * @brief  Receive a file using the ymodem protocol with CRC16.
 * @param  p_size The size of the file.
 * @retval COM_StatusTypeDef result of reception/programming
 */
COM_StatusTypeDef Ymodem_Receive(uint32_t *p_size)
{
	uint32_t i, packet_length, session_done = 0, file_done, errors = 0, session_begin = 0;
	uint32_t flashdestination, filesize;
	uint8_t *file_ptr;
	uint8_t file_size[FILE_SIZE_LENGTH], packets_received;
	COM_StatusTypeDef result = COM_OK;
	uint32_t tick_start = HAL_GetTick();
	uint32_t timeout_ms = 2000; // timeout 5 giây

	uint8_t decryptData[PACKET_1K_SIZE]; // bộ đệm tạm để lưu dữ liệu đã giải mã
	uint8_t remainData[FLASH_PAGE_SIZE]; // phần dư khi ghi flash chưa đủ block
	uint8_t writeData[PACKET_1K_SIZE];
	uint32_t dataLengthNeedProcees = 0; // tổng số byte firmware
	uint32_t remainByte = 0;
	uint32_t actualDataWrite = 0;
	FirmwareHeader_t fw_header;

	flashdestination = APP_START_ADDR;

	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, aes_key, aes_iv);

	while ((session_done == 0) && (result == COM_OK))
	{
		packets_received = 0;
		file_done = 0;
		while ((file_done == 0) && (result == COM_OK))
		{
			switch (ReceivePacket(aPacketData, &packet_length, DOWNLOAD_TIMEOUT))
			{
			case HLD_UART_OK:
				errors = 0;
				tick_start = HAL_GetTick();
				switch (packet_length)
				{
				case 2:
					/* Abort by sender */
					Serial_PutByte(ACK);
					result = COM_ABORT;
					break;
				case 0:
					/* End of transmission */
					Serial_PutByte(ACK);
					file_done = 1;
					break;
				default:
					/* Normal packet */
					if (aPacketData[PACKET_NUMBER_INDEX] != packets_received)
					{
						Serial_PutByte(NAK);
					}
					else
					{
						if (packets_received == 0)
						{
							/* File name packet */
							if (aPacketData[PACKET_DATA_INDEX] != 0)
							{
								/* File name extraction */
								i = 0;
								file_ptr = aPacketData + PACKET_DATA_INDEX;
								while ((*file_ptr != 0) && (i < FILE_NAME_LENGTH))
								{
									aFileName[i++] = *file_ptr++;
								}

								/* File size extraction */
								aFileName[i++] = '\0';
								i = 0;
								file_ptr++;
								while ((*file_ptr != ' ') && (i < FILE_SIZE_LENGTH))
								{
									file_size[i++] = *file_ptr++;
								}
								file_size[i++] = '\0';
								Str2Int(file_size, &filesize);

								/* Test the size of the image to be sent */
								/* Image size is greater than Flash size */
								if (*p_size > (APP_FLASH_SIZE + 1))
								{
									/* End session */
									uint8_t ca_buf[2] = {CA, CA};
									HLD_UART_Transmit(&UART_BOOTLOADER, ca_buf, 2, NAK_TIMEOUT);
									result = COM_LIMIT;
								}
								/* erase user application area */
								FLASH_If_Erase(APP_START_ADDR);
								*p_size = filesize;

								Serial_PutByte(ACK);
								Serial_PutByte(CRC16);
							}
							/* File header packet is empty, end session */
							else
							{
								Serial_PutByte(ACK);
								file_done = 1;
								session_done = 1;
								break;
							}
						}
						else /* Data packet */
						{
							if (packets_received == FIRST_DATA_PACKET_IDX)
							{
								uint8_t *ramsource = &aPacketData[PACKET_DATA_INDEX];

								memcpy(&fw_header, ramsource, sizeof(FirmwareHeader_t)); // phần chưa mã hóa

								if (fw_header.firmwareType != MY_FIRMWARE_TYPE)
								{
									printf("\r\nFirmware type sai\r\n");
									return COM_ABORT;
								}

								// Giải mã phần sau header
								uint8_t *ciphertext = ramsource + sizeof(FirmwareHeader_t);
								uint32_t cipher_len = packet_length - sizeof(FirmwareHeader_t);

								AES_CBC_decrypt_buffer(&ctx, ciphertext, cipher_len);
								memcpy(decryptData, ciphertext, cipher_len); // copy lại dữ liệu đã giải mã

								memcpy(&fw_header, decryptData, sizeof(FirmwareHeader_t)); // header đã giải mã
								dataLengthNeedProcees = fw_header.firmwareSize;

								printf("FirmwareSize = %lu\n", fw_header.firmwareSize);
								printf("Version = 0x%08lX\n", fw_header.firmwareVersion);

								// Bắt đầu ghi phần còn lại (sau 2 header)
								uint8_t *data_start = decryptData + sizeof(FirmwareHeader_t);
								uint16_t data_len = cipher_len - sizeof(FirmwareHeader_t);

								remainByte = data_len % FLASH_PAGE_SIZE;
								actualDataWrite = data_len - remainByte;

								if (remainByte)
									memcpy(remainData, data_start + actualDataWrite, remainByte);

								FLASH_If_Write(flashdestination, data_start, actualDataWrite);
								flashdestination += actualDataWrite;
								dataLengthNeedProcees -= actualDataWrite;
							}
							else
							{
								uint8_t *ramsource = &aPacketData[PACKET_DATA_INDEX];

								AES_CBC_decrypt_buffer(&ctx, ramsource, packet_length);
								memcpy(decryptData, ramsource, packet_length);

								uint32_t dataToWrite = (dataLengthNeedProcees < packet_length) ? dataLengthNeedProcees : packet_length;

								if (remainByte > 0)
								{
									uint16_t newLen = dataToWrite - remainByte;
									memcpy(writeData, remainData, remainByte);
									memcpy(writeData + remainByte, decryptData, newLen);
									FLASH_If_Write(flashdestination, writeData, remainByte + newLen);
									flashdestination += remainByte + newLen;
									dataLengthNeedProcees -= remainByte + newLen;

									remainByte = dataToWrite - newLen;
									if (dataLengthNeedProcees > 0 && dataLengthNeedProcees <= remainByte)
									{
										FLASH_If_Write(flashdestination, decryptData + newLen, dataLengthNeedProcees);
										dataLengthNeedProcees = 0;
									}
									else
									{
										memcpy(remainData, decryptData + newLen, remainByte);
									}
								}
								else
								{
									remainByte = dataToWrite % FLASH_PAGE_SIZE;
									actualDataWrite = dataToWrite - remainByte;

									FLASH_If_Write(flashdestination, decryptData, actualDataWrite);
									flashdestination += actualDataWrite;
									dataLengthNeedProcees -= actualDataWrite;

									if (remainByte > 0)
										memcpy(remainData, decryptData + actualDataWrite, remainByte);
								}
							}
						}
						packets_received++;
						session_begin = 1;
					}
					break;
				}
				break;
			case HLD_UART_BUSY: /* Abort actually */
				Serial_PutByte(CA);
				Serial_PutByte(CA);
				result = COM_ABORT;
				break;
			default:
				if (session_begin > 0)
				{
					errors++;
				}

				if ((HAL_GetTick() - tick_start) > timeout_ms)
				{
					result = COM_ABORT;
					break;
				}

				if (errors > MAX_ERRORS)
				{
					/* Abort communication */
					Serial_PutByte(CA);
					Serial_PutByte(CA);
				}
				else
				{
					Serial_PutByte(CRC16); /* Ask for a packet */
				}
				break;
			}
		}
	}
	return result;
}

