#include <stdio.h>
#include <stdint.h>
#include "evmomapl138_i2c.h"
#include "i2c.h"
#include "evmomapl138.h"

#define INTMUX1             *( volatile uint32_t* )( 0x01800104 )
#define MPU6050_SLAVE_ADDR     0x68

// Hardware register definitions mapping to C674x Core
extern  cregister volatile unsigned int CSR;    // Control Status Register
extern  cregister volatile unsigned int IER;    // Interrupt Enable Register
extern  cregister volatile unsigned int ISTP;   // Interrupt Service Table Pointer
extern  cregister volatile unsigned int ICR;

// Global driver states linked from your I2C driver file
extern volatile i2c_status_t g_driver_status;
extern volatile uint8_t g_rx_data[14];

// Global parsing buffers
int16_t raw_accel_x, raw_accel_y, raw_accel_z;
int16_t raw_temp;
int16_t raw_gyro_x,  raw_gyro_y,  raw_gyro_z;

// Forward declaration of the assembly interrupt vector table
extern void vectors(void);

int main(void) {
    // Local configuration variables grouped strictly at the top of the function scoop
    uint8_t wakeup[2] = {0x6B, 0x00};
    uint8_t read_start = 0x3B;
    volatile uint32_t delay;

    CSR &= ~(0x3);

    // 1. Assign the location of vector table in memory
    ISTP = (unsigned int)vectors;

    // 2. Map I2C0 INT (Event 36) to CPU Interrupt 4 (INT4)
    INTMUX1 = 0x24;

    I2C_init(I2C0, I2C_CLK_400K);
    printf("yes\n\n");

    volatile uint32_t dummy_flush = I2C0->ICIVR;

    ICR = 0xFFF0;

    // 3. Enable INT4 in the Interrupt Enable Register
    IER |= 0x0012;

    // 4. Enable Global Interrupts (Set GIE bit in CSR)
    CSR |= 0x0001;

    // 5. Initialize I2C0 Peripheral hardware engine at 400kHz
    I2C0->ICIMR = 0x0;
    I2C_init(I2C0, I2C_CLK_400K);
    //printf("yes");

    // Initialize state status machine
    g_driver_status = I2C_IDLE;

    // 6. Wake up the MPU6050 Sensor out of power-down sleep mode
    i2c_interrupt_write(MPU6050_SLAVE_ADDR, wakeup, 2);

    // Stall CPU until the non-blocking wake up sequence is completely written out
    while(g_driver_status == I2C_BUSY) {
        // Explicitly wait for the bus line state to clear
    }

    printf("MPU6050 Initialized. Starting continuous reading loop...\n\n");

    // Execution Core Loop
    while(1) {
        // Reset status tracking flag back to clean baseline before starting
        g_driver_status = I2C_IDLE;

        // Trigger our non-blocking combined write-then-read sequence for 14 bytes
        i2c0_interrupt_read(MPU6050_SLAVE_ADDR, &read_start, 14);

        // Keep core active while non-blocking ISR processes data frames across bus
        while(g_driver_status == I2C_BUSY) {
            // Background tasks or low-priority telemetry updates can execute here
        }

        if(g_driver_status == I2C_SUCCESS) {
            // Reconstruct the big-endian pairs (High Byte << 8) | Low Byte
            raw_accel_x = (int16_t)((g_rx_data[0] << 8)  | g_rx_data[1]);
            raw_accel_y = (int16_t)((g_rx_data[2] << 8)  | g_rx_data[3]);
            raw_accel_z = (int16_t)((g_rx_data[4] << 8)  | g_rx_data[5]);

            //raw_temp    = (int16_t)((g_rx_data[6] << 8)  | g_rx_data[7]);

            raw_gyro_x  = (int16_t)((g_rx_data[8] << 8)  | g_rx_data[9]);
            raw_gyro_y  = (int16_t)((g_rx_data[10] << 8) | g_rx_data[11]);
            raw_gyro_z  = (int16_t)((g_rx_data[12] << 8) | g_rx_data[13]);

            // Print raw sensor register fields directly to the CCS JTAG CIO Console Window
            printf("ACCEL: X=%6d | Y=%6d | Z=%6d  ||  GYRO: X=%6d | Y=%6d | Z=%6d\n",
                   raw_accel_x, raw_accel_y, raw_accel_z,
                   raw_gyro_x,  raw_gyro_y,  raw_gyro_z);
        }
        else if(g_driver_status == I2C_ERROR) {
            printf("[DRIVER ERROR]: I2C Transaction Failed (NACK received or Bus Error)\n");
        }

        // Software loop throttle to stabilize JTAG link stream pipeline
        for(delay = 0; delay < 500000; delay++) {
            // Clear loop pass
        }
    }

    //return 0;
}
