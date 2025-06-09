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

FirmwareHeader_t fw_header;
structVersion_t fw_ver;

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

COM_StatusTypeDef Ymodem_Receive(uint32_t *p_size)
{
	uint32_t i, packet_length, session_done = 0, file_done, errors = 0, session_begin = 0;
	uint32_t flashdestination, filesize;
	uint8_t *file_ptr;
	uint8_t file_size[FILE_SIZE_LENGTH], packets_received;
	COM_StatusTypeDef result = COM_OK;
	uint32_t tick_start = HAL_GetTick();
	uint32_t Bootloader_Timeout = BOOTLOADER_TIMEOUT_MS;

	uint8_t decryptData[PACKET_1K_SIZE]; // bộ đệm tạm để lưu dữ liệu đã giải mã
	uint8_t remainData[FLASH_PAGE_SIZE]; // phần dư khi ghi flash chưa đủ block
	uint8_t writeData[PACKET_1K_SIZE];
	uint32_t dataLengthNeedProcees = 0; // tổng số byte firmware
	uint32_t remainByte = 0;
	uint32_t actualDataWrite = 0;

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
								memcpy(&fw_header, ramsource, sizeof(FirmwareHeader_t));

								if (fw_header.firmwareType != MY_FIRMWARE_TYPE)
								{
									return COM_ERROR;
								}

								uint8_t *ciphertext = ramsource + sizeof(FirmwareHeader_t);
								uint32_t cipher_len = packet_length - sizeof(FirmwareHeader_t);

								AES_CBC_decrypt_buffer(&ctx, ciphertext, cipher_len);
								memcpy(decryptData, ciphertext, cipher_len);

								memcpy(&fw_header, decryptData, sizeof(FirmwareHeader_t));
								dataLengthNeedProcees = fw_header.firmwareSize;

								FLASH_If_Erase(FW_HEADER_ADDR);
								FLASH_If_Write(FW_HEADER_ADDR, (uint32_t *)&fw_header, sizeof(FirmwareHeader_t) / 4);

								uint8_t *data_start = decryptData + sizeof(FirmwareHeader_t);
								uint16_t data_len = cipher_len - sizeof(FirmwareHeader_t);

								remainByte = data_len % FLASH_PAGE_SIZE;
								actualDataWrite = data_len - remainByte;

								if (actualDataWrite % 4 != 0)
								{
									uint8_t pad = 4 - (actualDataWrite % 4);
									memset(data_start + actualDataWrite, 0, pad);
									actualDataWrite += pad;
								}

								if (remainByte > 0)
								{
									memcpy(remainData, data_start + actualDataWrite, remainByte);
								}
								
								// Ghi flash phần chính
								if(FLASH_If_Write(flashdestination, (uint32_t *)data_start, (actualDataWrite / 4)) == FLASHIF_OK)
								{
									flashdestination += actualDataWrite;
									dataLengthNeedProcees -= actualDataWrite;
									Serial_PutByte(ACK);
								}
								else
								{
									Serial_PutByte(CA);
									Serial_PutByte(CA);
									result = COM_DATA;
								}
							}
							else
							{
								uint8_t *ramsource = &aPacketData[PACKET_DATA_INDEX];

								// Giải mã và copy lại để xử lý
								AES_CBC_decrypt_buffer(&ctx, ramsource, packet_length);
								memcpy(decryptData, ramsource, packet_length);

								uint32_t dataToWrite = (dataLengthNeedProcees < packet_length) ? dataLengthNeedProcees : packet_length;

								if (remainByte > 0)
								{
									uint16_t newLen = dataToWrite - remainByte;

									memcpy(writeData, remainData, remainByte);
									memcpy(writeData + remainByte, decryptData, newLen);

									uint32_t totalWrite = remainByte + newLen;

									if (totalWrite % 4 != 0)
									{
										uint8_t pad = 4 - (totalWrite % 4);
										memset(writeData + totalWrite, 0, pad);
										totalWrite += pad;
									}

									if(FLASH_If_Write(flashdestination, (uint32_t *)writeData, (totalWrite / 4)) == FLASHIF_OK)
									{
										flashdestination += totalWrite;
										dataLengthNeedProcees -= totalWrite;
									}
									else
									{
										Serial_PutByte(CA);
										Serial_PutByte(CA);
										result = COM_DATA;
									}

									remainByte = dataToWrite - newLen;

									if (dataLengthNeedProcees > 0 && dataLengthNeedProcees <= remainByte)
									{
										uint32_t lastLen = dataLengthNeedProcees;

										// Pad nếu cần
										if (lastLen % 4 != 0)
										{
											uint8_t pad = 4 - (lastLen % 4);
											memset(decryptData + newLen + lastLen, 0, pad);
											lastLen += pad;
										}

										if (FLASH_If_Write(flashdestination, (uint32_t *)(decryptData + newLen), (lastLen / 4)) == FLASHIF_OK)
										{
											dataLengthNeedProcees = 0;
										}
										else
										{
											Serial_PutByte(CA);
											Serial_PutByte(CA);
											result = COM_DATA;
										}
									}
									else if (remainByte > 0)
									{
										memcpy(remainData, decryptData + newLen, remainByte);
									}
								}
								else
								{
									remainByte = dataToWrite % FLASH_PAGE_SIZE;
									actualDataWrite = dataToWrite - remainByte;

									// Pad nếu actualDataWrite không chia hết 4
									if (actualDataWrite % 4 != 0)
									{
										uint8_t pad = 4 - (actualDataWrite % 4);
										memset(decryptData + actualDataWrite, 0, pad);
										actualDataWrite += pad;
									}

									if (FLASH_If_Write(flashdestination, (uint32_t *)decryptData, (actualDataWrite / 4)) == FLASHIF_OK)
									{
										flashdestination += actualDataWrite;
										dataLengthNeedProcees -= actualDataWrite;
									}
									else
									{
										Serial_PutByte(CA);
										Serial_PutByte(CA);
										result = COM_DATA;
									}

									if (remainByte > 0)
									{
										memcpy(remainData, decryptData + actualDataWrite, remainByte);
									}
								}
								Serial_PutByte(ACK);
							}
						}
						session_begin = 1;
						packets_received++;
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

				if ((HAL_GetTick() - tick_start) > Bootloader_Timeout)
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

