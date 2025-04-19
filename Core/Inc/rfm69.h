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


/*------------------------------------------------------------------------------
	CONSTANTS
------------------------------------------------------------------------------*/

#define RFM69_MODE_SLEEP	0
#define RFM69_MODE_STANDBY	1
#define RFM69_MODE_FS		2
#define RFM69_MODE_TX		3
#define RFM69_MODE_RX		4

#define RFM69_DI0_RX_PAYLOAD_READY 	1
#define RFM69_DI0_TX_NONE			2

#define RFM69_TIMEOUT_MS	4000


/*------------------------------------------------------------------------------
	TYPE DEFINITIONS
------------------------------------------------------------------------------*/

/**
 * @brief GPIO pin state structure.
 */
struct pin_state
{
	GPIO_TypeDef *port;
	uint16_t pin;
};

/**
 * @brief RFM69 structure.
 * This structure contains the configuration of the RFM69 module.
 */
typedef struct RFM69
{
	SPI_HandleTypeDef *spi; /**< SPI peripheral */

	struct pin_state reset; /**< SPI Reset GPIO */
	struct pin_state cs; /**< SPI CS GPIO */

	uint8_t high_power_en; /**< High power mode module compatibility */

	uint8_t _listen_mode_activated; /**< Listen mode activated internal flag */
} RFM69_t;


/*------------------------------------------------------------------------------
	DECLARATIONS
------------------------------------------------------------------------------*/

/**
 * @brief Initialize the RFM69 module. The base configuration is sent to the module.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 */
extern void RFM69_Init(RFM69_t *rfm69);

/**
 * @brief Send a custom configuration to the RFM69 module.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param config Pointer to the configuration array.
 * @param config_size Size of the configuration array.
 */
extern void RFM69_SetCustomConfig(RFM69_t *rfm69, const uint8_t config[][2], size_t config_length);

/**
 * @brief Change the mode of the RFM69 module.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param mode Mode to set (see rfm69.h for available modes).
 */
extern void RFM69_SetMode(RFM69_t *rfm69, uint8_t mode);

/**
 * @brief Change the DI0 interrupt mapping of the RFM69 module.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param mapping Mapping to set (see rfm69.h for available mappings).
 */
extern void RFM69_ChangeDI0Mapping(RFM69_t *rfm69, uint8_t mapping);

/**
 * @brief Activate the listen mode of the RFM69 module.
 * This mode allows the module to discontinuously listen for incoming packets.
 * See main.c or RFM69 datasheet for the configuration parameters.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param resol_idle Resolution for the listen idle mode
 * @param coef_idle Coefficient for the listen idle mode
 * @param resol_rx Resolution for the listen RX mode
 * @param coef_rx Coefficient for the listen RX mode
 */
extern void RFM69_ActiveListenMode(RFM69_t *rfm69, uint8_t resol_idle, uint8_t coef_idle, uint8_t resol_rx, uint8_t coef_rx);

/**
 * @brief Disable the listen mode of the RFM69 module.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param mode Mode to set after disabling listen mode (see rfm69.h for available modes).
 */
extern void RFM69_DisableListenMode(RFM69_t *rfm69, uint8_t mode);

/**
 * @brief Send a message over the air using the RFM69 module.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param message Pointer to the message to send.
 * @param message_size Size of the message to send.
 */
extern void RFM69_SendMessage(RFM69_t *rfm69, uint8_t *message, size_t message_lenght);

/**
 * @brief Set the output power of the RFM69 module.
 * Output power of module is from -18 dBm to +13 dBm in "low" power devices, -2 dBm to +20 dBm in high power devices
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param dBm Output power in dBm.
 * @return 0 on success, -1 if dBm is out of range, -2 if high power is not enabled and dBm > 13, -3 if high power is enabled and dBm < -2.
 */
extern int RFM69_SetPowerDBm(RFM69_t *rfm69, int8_t dBm);

/**
 * @brief Active receive mode and try to read a message from the RFM69 FIFO.
 * The module stay in RX mode after the function call if listen mode is not activated.
 * 
 * @param rfm69 Pointer to the RFM69 structure.
 * @param buffer Pointer to the buffer to store the received message.
 * @param buffer_size Size of the buffer.
 * @return Number of bytes read from the RFM69 FIFO.
 */
extern size_t RFM69_ReceiveMessage(RFM69_t *rfm69, uint8_t *buffer, size_t buffer_size);

#endif /* INC_RFM69_H_ */
