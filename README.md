# Light Doorbell Software

Firmware for the **Light Doorbell** project, designed with a STM32L051K8T6 microcontroller and a RFM69HCW radio module. 

The code is written in C using the STM32 CubeHAL framework.

## Features

- Sends a message containing a predefined code when the button is pressed
- Listens for and decodes such messages to trigger the doorbell
- Same firmware runs on both the transmitter and receiver board
- Basic driver implementation for the RFM69:
  - Register access
  - Mode configuration
  - Message transmission and reception
  - Output power configuration
  - DI0 interrupt mapping
- IRQ management (external interrupt from the RFM69 DI0 pin and the user switch)
- Battery voltage measurement
- Power saving management:
  - STM32 stop mode
  - RFM69 listen mode
- Dual-color LEDs that change color depending on the battery voltage level (green/red)

## Energy consumption

Power efficiency was a key objective, as the system is battery-powered (with a 400 mAh Li-ion).  

The following consumption values were measured using the firmware (without UART) on the final custom PCB:

| Scenario                            | Voltage | Current   | Power     | Energy              |
|-------------------------------------|---------|-----------|-----------|---------------------|
| Listen mode                         | 4.2 V   | 90 µA     | 378 µW    | Depends on duration |
| Message transmission + LED blink    | 4.2 V   | 141 mA    | 592 mW    | 178 mJ              |

These values allow the system to run for several weeks without recharging, assuming normal usage...  

Further optimizations are possible, such as:
- Adjusting listen mode duty cycle (increases transmission time but reduces power consumption during reception)
- Reducing LED blink duration

## Related projects and documentation

- **[Light Doorbell HW](https://github.com/Estylos/Light-Doorbell-HW): Hardware design of the project**
- [RFM69HCW Datasheet](https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf)
- [RFM69-STM32 by André Heßling](https://github.com/ahessling/RFM69-STM32): Project that inspired the base of my RFM69 driver
