/**
 * @file        rfm69.h
 * @brief       RFM69 driver
 * @author      Esteban CADIC
 * @author      André Heßling
 * @version     1.0
 * @date        2025
 * @copyright   MIT License
 *
 */

#include "rfm69.h"

#include <stdio.h>

static inline void SPI_ChipSelect(RFM69_t *rfm69);
static inline void SPI_ChipUnselect(RFM69_t *rfm69);
static void SPI_SendData(RFM69_t *rfm69, uint8_t addr, uint8_t *data, uint16_t data_size);
static void SPI_ReadData(RFM69_t *rfm69, uint8_t addr, uint8_t *data, uint16_t data_size);
static void WriteRegister(RFM69_t *rfm69, uint8_t reg, uint8_t byte);
static uint8_t ReadRegister(RFM69_t *rfm69, uint8_t reg);
static uint8_t ReadMode(RFM69_t *rfm69);
static void WaitForModeReady(RFM69_t *rfm69);
static void WaitForPacketSent(RFM69_t *rfm69);

static const uint8_t rfm69_base_config[][2] = {
		{ 0x01, 0x04 }, // RegOpMode: Standby Mode
		{ 0x02, 0x00 }, // RegDataModul: Packet mode, FSK, no shaping
		{ 0x03, 0x0C }, // RegBitrateMsb: 10 kbps
		{ 0x04, 0x80 }, // RegBitrateLsb
		{ 0x05, 0x01 }, // RegFdevMsb: 20 kHz
		{ 0x06, 0x48 }, // RegFdevLsb
		{ 0x07, 0x6C }, // RegFrfMsb: 433,42 MHz
		{ 0x08, 0x5A }, // RegFrfMid
		{ 0x09, 0xE1 }, // RegFrfLsb
		{ 0x18, 0x88 }, // RegLNA: 200 Ohm impedance, gain set by AGC loop
		{ 0x19, 0x4C }, // RegRxBw: 25 kHz
		{ 0x2C, 0x00 }, // RegPreambleMsb: 3 bytes preamble
		{ 0x25, 0x40 }, // RegDioMapping1: DIO0 PayloadReady
		{ 0x2D, 0x03 }, // RegPreambleLsb
		{ 0x2E, 0x88 }, // RegSyncConfig: Enable sync word, 2 bytes sync word
		{ 0x2F, 0x20 }, // RegSyncValue1: 0x2025
		{ 0x30, 0x25 }, // RegSyncValue2
		{ 0x37, 0x50 }, // RegPacketConfig1: Fixed length, CRC on, whitening
		{ 0x38, 0x01 }, // RegPayloadLength: 1 bytes payload
		{ 0x3C, 0x80 }, // RegFifoThresh: TxStart on FifoNotEmpty, 0 bytes FifoThreshold
		{ 0x58, 0x1B }, // RegTestLna: Normal sensitivity mode
		};

void RFM69_Init(RFM69_t *rfm69)
{
	rfm69->_listen_mode_activated = 0;

	RFM69_SetCustomConfig(rfm69, rfm69_base_config, sizeof(rfm69_base_config) / 2);

	// Disable OCP for high power devices, enable otherwise
	WriteRegister(rfm69, 0x13, 0x0A | (rfm69->high_power_en ? 0x00 : 0x10));
}

void RFM69_SetCustomConfig(RFM69_t *rfm69, const uint8_t config[][2], size_t config_size)
{
	for(size_t i = 0; i < config_size; i++)
		WriteRegister(rfm69, config[i][0], config[i][1]);
}

void RFM69_SetMode(RFM69_t *rfm69, uint8_t mode)
{
	if((mode == ReadMode(rfm69)) || (mode > RFM69_MODE_RX))
		return;

	WriteRegister(rfm69, 0x01, mode << 2);
}

void RFM69_ChangeDI0Mapping(RFM69_t *rfm69, uint8_t mapping)
{
	WriteRegister(rfm69, 0x25, mapping << 6);
}

void RFM69_ActiveListenMode(RFM69_t *rfm69, uint8_t resol_idle, uint8_t coef_idle, uint8_t resol_rx, uint8_t coef_rx)
{
	uint8_t reg_listen_1 = 0x02 << 1; // ListenEnd: 10, resume Listen Mode
	reg_listen_1 |= (resol_rx & 0x03) << 4; // ListenResolRx
	reg_listen_1 |= (resol_idle & 0x03) << 6; // ListenResolIdle

	uint8_t listen_mode_config[][2] = {
			{ 0x01, 0x44 }, // RegOpMode: ListenOn, Standby Mode
			{ 0x0D, reg_listen_1 }, // RegListen1
			{ 0x0E, coef_idle }, // RegListen2
			{ 0x0F, coef_rx }, // RegListen3
			{ 0x29, 0xB4 }, // RssiThreshold: -90 dB
			{ 0x2A, 0x3E }, // TimeoutRxStart: 62*16*(1/10kbps) = 100 ms
			{ 0x2B, 0x3E } // TimeoutRssiThresh: 100 ms
	};

	RFM69_SetCustomConfig(rfm69, listen_mode_config, sizeof(listen_mode_config) / 2);

	rfm69->_listen_mode_activated = 1;
}

void RFM69_DisableListenMode(RFM69_t *rfm69, uint8_t mode)
{
	if(mode > RFM69_MODE_RX)
		return;

	WriteRegister(rfm69, 0x01, 0x20 | mode << 2); // RegOpMode: ListenAbort, selected mode
	WriteRegister(rfm69, 0x01, mode << 2); // RegOpMode: selected mode

	rfm69->_listen_mode_activated = 0;
}

void RFM69_SendMessage(RFM69_t *rfm69, uint8_t *message, size_t message_size)
{
	RFM69_SetMode(rfm69, RFM69_MODE_STANDBY);
	WaitForModeReady(rfm69);

	// Clear FIFO
	WriteRegister(rfm69, 0x28, 0x10);

	if(message_size == 0)
		return;

	// Send message to RFM69 FIFO
	SPI_ChipSelect(rfm69);
	SPI_SendData(rfm69, 0x00 | 0x80, message, message_size);
	SPI_ChipUnselect(rfm69);

	// Transmit the message and wait for it
	RFM69_SetMode(rfm69, RFM69_MODE_TX);
	WaitForPacketSent(rfm69);

	RFM69_SetMode(rfm69, RFM69_MODE_SLEEP);
	WaitForModeReady(rfm69);
}

int RFM69_SetPowerDBm(RFM69_t *rfm69, int8_t dBm)
{
	uint8_t power_level = 0;

	/* Output power of module is from -18 dBm to +13 dBm
	 * in "low" power devices, -2 dBm to +20 dBm in high power devices */

	if(dBm < -18 || dBm > 20)
		return -1;

	if(rfm69->high_power_en == 0 && dBm > 13)
		return -2;

	if(rfm69->high_power_en == 1 && dBm < -2)
		return -3;

	if(rfm69->high_power_en == 1)
	{
		if(dBm >= -2 && dBm <= 13)
		{
			// Pout = -18 + OutputPower [dBm], with PA1 enabled on pin PA_BOOST
			power_level = dBm + 18;

			// Enable PA1 only
			WriteRegister(rfm69, 0x11, 0x40 | power_level);

			// Disable high power settings
			WriteRegister(rfm69, 0x5A, 0x55);
			WriteRegister(rfm69, 0x5C, 0x70);
		}
		else if(dBm > 13 && dBm <= 17)
		{
			// Pout = -14 + OutputPower [dBm], with PA1 and PA2 combined on pin PA_BOOST
			power_level = dBm + 14;

			// Enable PA1 & PA2
			WriteRegister(rfm69, 0x11, 0x60 | power_level);

			// Disable high power settings
			WriteRegister(rfm69, 0x5A, 0x55);
			WriteRegister(rfm69, 0x5C, 0x70);
		}
		else
		{
			// Pout = -11 + OutputPower [dBm], with PA1 and PA2 on PA_BOOST and high Power PA settings
			power_level = dBm + 11;

			// Enable PA1 & PA2
			WriteRegister(rfm69, 0x11, 0x60 | power_level);

			// Enable high power settings
			WriteRegister(rfm69, 0x5A, 0x5D);
			WriteRegister(rfm69, 0x5C, 0x7C);
		}
	}
	else // High power not enabled
	{
		// Pout = -18 + OutputPower [dBm], with PA0 output on pin RFIO
		power_level = dBm + 18;

		// Enable PA0 only
		WriteRegister(rfm69, 0x11, 0x80 | power_level);
	}

	return 0;
}

size_t RFM69_ReceiveMessage(RFM69_t *rfm69, uint8_t *buffer, size_t buffer_size)
{
	size_t bytes_read = 0;
	if(ReadMode(rfm69) != RFM69_MODE_RX && !rfm69->_listen_mode_activated)
	{
		RFM69_SetMode(rfm69, RFM69_MODE_RX);
		WaitForModeReady(rfm69);
	}

	// If PayloadReady flag is set
	if(ReadRegister(rfm69, 0x28) & 0x04 || rfm69->_listen_mode_activated)
	{
		printf("PayloadReady flag is set \n");
		RFM69_SetMode(rfm69, RFM69_MODE_STANDBY);

		// Read until FIFO is empty or buffer size is reached
		while((ReadRegister(rfm69, 0x28) & 0x40) && (bytes_read < buffer_size))
		{
			buffer[bytes_read] = ReadRegister(rfm69, 0x00);
			bytes_read++;
		}

		if(!rfm69->_listen_mode_activated)
		{
			RFM69_SetMode(rfm69, RFM69_MODE_RX);
			WaitForModeReady(rfm69);
		}
	}

	return bytes_read;
}

static inline void SPI_ChipSelect(RFM69_t *rfm69)
{
	HAL_GPIO_WritePin(rfm69->cs.port, rfm69->cs.pin, GPIO_PIN_RESET);
}

static inline void SPI_ChipUnselect(RFM69_t *rfm69)
{
	HAL_GPIO_WritePin(rfm69->cs.port, rfm69->cs.pin, GPIO_PIN_SET);
}

static void SPI_SendData(RFM69_t *rfm69, uint8_t addr, uint8_t *data, uint16_t data_size)
{
	// REGISTER
	HAL_SPI_Transmit(rfm69->spi, &addr, 1, HAL_MAX_DELAY);

	// DATA
	if(data_size != 0)
		HAL_SPI_Transmit(rfm69->spi, data, data_size, HAL_MAX_DELAY);
}

static void SPI_ReadData(RFM69_t *rfm69, uint8_t addr, uint8_t *data, uint16_t data_size)
{
	// REGISTER
	HAL_SPI_Transmit(rfm69->spi, &addr, 1, HAL_MAX_DELAY);

	// DATA
	if(data_size != 0)
		HAL_SPI_Receive(rfm69->spi, data, data_size, HAL_MAX_DELAY);
}

static void WriteRegister(RFM69_t *rfm69, uint8_t reg, uint8_t byte)
{
	uint8_t reg_array[2] = { reg | 0x80, byte }; // 0x80 to set the write flag

	SPI_ChipSelect(rfm69);
	SPI_SendData(rfm69, reg_array[0], &reg_array[1], 1);
	SPI_ChipUnselect(rfm69);
}

static uint8_t ReadRegister(RFM69_t *rfm69, uint8_t reg)
{
	uint8_t reg_value = 0;

	SPI_ChipSelect(rfm69);
	SPI_ReadData(rfm69, reg, &reg_value, 1);
	SPI_ChipUnselect(rfm69);

	return reg_value;
}

static uint8_t ReadMode(RFM69_t *rfm69)
{
	return (ReadRegister(rfm69, 0x01) >> 2) & 0x07;
}

static void WaitForModeReady(RFM69_t *rfm69)
{
	uint32_t time_entry = HAL_GetTick();

	// Wait until ModeReady bit is set
	while((ReadRegister(rfm69, 0x27) & 0x80) == 0 && (HAL_GetTick() - time_entry) < RFM69_TIMEOUT_MS)
		;
}

static void WaitForPacketSent(RFM69_t *rfm69)
{
	uint32_t time_entry = HAL_GetTick();

	// Wait until PacketSent bit is set
	while((ReadRegister(rfm69, 0x28) & 0x08) == 0 && (HAL_GetTick() - time_entry) < RFM69_TIMEOUT_MS)
		;
}
