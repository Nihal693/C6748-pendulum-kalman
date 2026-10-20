/*
 * i2c.c
 *
 *  Created on: Jun 25, 2026
 *      Author: nihalux
 */

#include <stdint.h>
#include "i2c.h"
#include "evmomapl138.h"

#define ICXRDY_EN     (1U << 4)
#define NACK_EN       (1U << 1)
#define ICRRDY_EN     (1U << 3)
#define SCD_EN        (1U << 5)
#define ARDY_EN       (1U << 2)
#define START_BIT     (1U << 13)
#define MASTER_MOD    (1U << 10)
#define TX_MOD        (1U << 9)
#define RESET_OUT     (1U << 5)
#define STOP_BIT      (1U << 11)

volatile uint16_t g_byte_left_tx;
volatile uint16_t g_byte_left_rx;
volatile uint8_t *g_tx_buff;
volatile uint8_t *g_rx_buff;
volatile uint8_t g_rx_data[14];
volatile i2c_status_t g_driver_status;
volatile op_t g_op_t;

volatile uint8_t int_src;

int i2c_interrupt_write(uint8_t slave_addr, uint8_t *data_arr, uint16_t length){
    // Check if bus busy
    if(g_driver_status == I2C_BUSY) return I2C_BUSY;
    // Update status and op type
    g_driver_status = I2C_BUSY;
    g_op_t = STANDALONE_WRITE;
    // Copy to global variables
    g_tx_buff = data_arr;
    g_byte_left_tx = length;
    // Slave address
    I2C0->ICSAR = slave_addr;
    // No. of bytes
    I2C0->ICCNT = length;
    // Enable transmit reg empty interrupt
    I2C0->ICIMR = ICXRDY_EN | NACK_EN |SCD_EN;
    // Config I2C mode reg
    I2C0->ICMDR = RESET_OUT | MASTER_MOD | TX_MOD | START_BIT | STOP_BIT;

    return 0;
}

int i2c0_interrupt_read(uint8_t slave_addr, uint8_t *reg_addr, uint8_t length){
    if(g_driver_status == I2C_BUSY) return I2C_BUSY;
    // Update status and op type
    g_driver_status = I2C_BUSY;
    g_op_t = COMBINED_READ;
    // Copy to global variables
    g_tx_buff = reg_addr;
    g_byte_left_rx = length;
    g_byte_left_tx = 1U;
    g_rx_buff = g_rx_data;
    // Slave address
    I2C0->ICSAR = slave_addr;
    // No. of bytes
    I2C0->ICCNT = 1U;
    // Enable transmit reg empty interrupt
    I2C0->ICIMR = ICXRDY_EN | NACK_EN | ARDY_EN | SCD_EN;
    // Config I2C mode reg
    I2C0->ICMDR = RESET_OUT | MASTER_MOD | TX_MOD | START_BIT;

    return 0;
}

interrupt void i2c_isr_handler(){
    // Check cause
    while((int_src = I2C0->ICIVR) != 0x0){

        switch(int_src){
            case 0x02:
                // later
                I2C0->ICMDR |= STOP_BIT;
                I2C0->ICIMR &= ~ICXRDY_EN;
                I2C0->ICSTR = 0x02;
                g_driver_status = I2C_ERROR;
                return;
            case 0x03:
                I2C0->ICIMR &= ~ICXRDY_EN;
                I2C0->ICCNT = g_byte_left_rx;
                I2C0->ICIMR |= ICRRDY_EN;
                I2C0->ICMDR = RESET_OUT | MASTER_MOD | START_BIT | STOP_BIT;
                break;
            case 0x04:
                if(g_byte_left_rx > 0){
                    *g_rx_buff = (uint8_t)I2C0->ICDRR;
                    g_rx_buff++;
                    g_byte_left_rx--;
                }
                /*if(g_byte_left_rx == 0){
                    I2C0->ICIMR &= ~ICRRDY_EN;
                    g_driver_status = I2C_SUCCESS;
                }*/
                break;
            case 0x05:
                // Check bytes left to send
                if(g_op_t == COMBINED_READ){
                    // for combined reading write the address of reg to read from
                    if(g_byte_left_tx > 0){
                        I2C0->ICDXR = *g_tx_buff;
                        g_byte_left_tx--;
                    }
                    /*else{
                        // Change mode to rx
                        I2C0->ICIMR &= ~ICXRDY_EN;
                        I2C0->ICCNT = g_byte_left_rx;
                        I2C0->ICIMR |= ICRRDY_EN;
                        I2C0->ICMDR = RESET_OUT | MASTER_MOD | START_BIT | STOP_BIT;
                    }*/
                }
                else{
                    if(g_byte_left_tx > 0){
                            I2C0->ICDXR = *g_tx_buff;
                            g_tx_buff++;
                            g_byte_left_tx--;
                    }
                }
                break;
            case 0x06:
                if((g_op_t == COMBINED_READ) && (g_byte_left_rx == 0)){
                    I2C0->ICIMR &= ~ICRRDY_EN;
                    g_driver_status = I2C_SUCCESS;
                }
                else if((g_op_t == STANDALONE_WRITE) && (g_byte_left_tx == 0)){
                    I2C0->ICIMR &= ~ICXRDY_EN;
                    g_driver_status = I2C_SUCCESS;
                }
                break;
            default:
                break;
        }
    }
}
