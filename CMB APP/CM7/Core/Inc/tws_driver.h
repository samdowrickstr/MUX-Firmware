/*
 * tws_driver.h
 *
 *  Created on: Apr 25, 2024
 *      Author: Sam.Dowrick
 */

#ifndef INC_TWS_DRIVER_H_
#define INC_TWS_DRIVER_H_

void TWS_WAIT(uint16_t tens_of_us);
void delay_us (uint16_t us);
void GPIO_SET_TWS_MODE_INPUT();
void GPIO_SET_MODE_NORMAL();
void _TWS_DRIVER_CLOCK_PULSE();
void _TWS_DRIVER_START();
void _TWS_DRIVER_OP_CODE_READ();
void _TWS_DRIVER_OP_CODE_WRITE();
void _TWS_DRIVER_SWITCH_REG_OUTPUT(uint8_t switch_id, uint8_t REG);
void _TWS_DRIVER_TA_WRITE();
void _TWS_DRIVER_TA_READ();
void _TWS_DRIVER_WRITE_DATA(uint16_t data);
uint16_t _TWS_DRIVER_READ_DATA();
void TWS_DRIVER_WRITE(uint8_t REG, uint16_t DATA);
uint16_t TWS_DRIVER_READ(uint8_t REG);
uint16_t TWS_PHY_READ(uint8_t startIdle, uint8_t miiRegAddr, uint8_t phyAddr);
void TWS_PHY_WRITE(uint8_t startIdle, uint8_t writeAll, uint8_t miiRegAddr, uint8_t phyAddr, uint16_t data);

#endif /* INC_TWS_DRIVER_H_ */
