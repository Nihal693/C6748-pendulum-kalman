/*
 * pwm.c
 *
 *  Created on: Jun 26, 2026
 *      Author: nihalux
 */


#include "pwm.h"
#include "evmomapl138_sysconfig.h"

#define HSPCLKDIV     (5U << 7)
#define CLKDIV        (6U << 10)
#define CLK_MODE      (3U << 0)
#define ZRO_BITS      (2U << 0)
#define CAU_BITS      (1U << 4)
#define TBCLKSYNC     (1U << 12)


void epwm1a_init(void)
{
    // Disable clock
    SYSCONFIG->CFGCHIP[1] &= ~TBCLKSYNC;

    // Configure the time base control reg
    EHRPWM1->TBCTL &= ~(HSPCLKDIV | CLKDIV);
    EHRPWM1->TBCTL |= (HSPCLKDIV | CLKDIV);
    EHRPWM1->TBCTL &= ~CLK_MODE;
    // Set pwm frequency
    EHRPWM1->TBPRD = 0x249E;
    // set trigger point
    EHRPWM1->CMPA = 0U;
    // Action when reaching tigger point
    EHRPWM1->AQCTLA = 0x0;
    EHRPWM1->AQCTLA |= ZRO_BITS;
    EHRPWM1->AQCTLA |= CAU_BITS;

    // Enable CLK
    SYSCONFIG->CFGCHIP[1] |= TBCLKSYNC;

}

void epwm1a_setDuty(uint16_t duty)
{
    EHRPWM1->CMPA = duty;
}
