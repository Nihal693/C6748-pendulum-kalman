/*
 * i2c.h
 *
 *  Created on: Jun 26, 2026
 *      Author: nihalux
 */

#ifndef I2C_H_
#define I2C_H_

#include "evmomapl138_i2c.h"

typedef enum{
    I2C_SUCCESS,
    I2C_ERROR,
    I2C_IDLE,
    I2C_BUSY
}i2c_status_t;

typedef enum{
    STANDALONE_WRITE,
    COMBINED_READ
}op_t;

extern volatile uint16_t g_byte_left;
extern volatile uint8_t *g_tx_buff;
extern volatile i2c_status_t g_driver_status;

int i2c_interrupt_write(uint8_t slave_addr, uint8_t *data_arr, uint16_t length);
int i2c0_interrupt_read(uint8_t slave_addr, uint8_t *reg_addr, uint8_t length);

#endif /* I2C_H_ */
