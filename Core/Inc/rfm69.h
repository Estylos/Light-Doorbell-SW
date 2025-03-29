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

#ifndef INC_RFM69_H_
#define INC_RFM69_H_

#include "main.h"

#define RFM69_MODE_SLEEP	0
#define RFM69_MODE_STANDBY	1
#define RFM69_MODE_FS		2
#define RFM69_MODE_TX		3
#define RFM69_MODE_RX		4

#define RFM69_DI0_RX_PAYLOAD_READY 	1
#define RFM69_DI0_TX_NONE			2

#define RFM69_TIMEOUT_MS	4000

struct pin_state
{
	GPIO_TypeDef *port;
	uint16_t pin;
};

typedef struct RFM69
{
	SPI_HandleTypeDef *spi; /**< SPI peripheral */

	struct pin_state reset; /**< Reset GPIO */
	struct pin_state cs; /**< CS GPIO */

	uint8_t high_power_en;

	uint8_t _listen_mode_activated;
} RFM69_t;

extern void RFM69_Init(RFM69_t *rfm69);
extern void RFM69_SetCustomConfig(RFM69_t *rfm69, const uint8_t config[][2], size_t config_length);
extern void RFM69_SetMode(RFM69_t *rfm69, uint8_t mode);
extern void RFM69_ChangeDI0Mapping(RFM69_t *rfm69, uint8_t mapping);
extern void RFM69_ActiveListenMode(RFM69_t *rfm69, uint8_t resol_idle, uint8_t coef_idle, uint8_t resol_rx, uint8_t coef_rx);
extern void RFM69_DisableListenMode(RFM69_t *rfm69, uint8_t mode);
extern void RFM69_SendMessage(RFM69_t *rfm69, uint8_t *message, size_t message_lenght);
extern int RFM69_SetPowerDBm(RFM69_t *rfm69, int8_t dBm);
extern size_t RFM69_ReceiveMessage(RFM69_t *rfm69, uint8_t *buffer, size_t buffer_size);

#endif /* INC_RFM69_H_ */
