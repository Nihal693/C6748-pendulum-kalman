/*
 * pwm.h
 *
 *  Created on: Jun 26, 2026
 *      Author: nihalux
 */

#ifndef PWM_H_
#define PWM_H_

#include "types.h"

typedef struct {
    volatile uint16_t TBCTL;      // 0x00
    volatile uint16_t TBSTS;      // 0x02
    volatile uint16_t TBPHSHR;    // 0x04
    volatile uint16_t TBPHS;      // 0x06
    volatile uint16_t TBCNT;      // 0x08
    volatile uint16_t TBPRD;      // 0x0A
    volatile uint16_t RESERVED1;  // 0x0C
    volatile uint16_t CMPCTL;     // 0x0E
    volatile uint16_t CMPAHR;     // 0x10
    volatile uint16_t CMPA;       // 0x12
    volatile uint16_t CMPB;       // 0x14
    volatile uint16_t AQCTLA;     // 0x16
    volatile uint16_t AQCTLB;     // 0x18
    volatile uint16_t AQSFRC;     // 0x1A
    volatile uint16_t AQCSFRC;    // 0x1C
    volatile uint16_t DBCTL;      // 0x1E
    volatile uint16_t DBRED;      // 0x20
    volatile uint16_t DBFED;      // 0x22
    volatile uint16_t TZSEL;      // 0x24
    volatile uint16_t RESERVED2;  // 0x26
    volatile uint16_t TZCTL;      // 0x28
    volatile uint16_t TZEINT;     // 0x2A
    volatile uint16_t TZFLG;      // 0x2C
    volatile uint16_t TZCLR;      // 0x2E
    volatile uint16_t TZFRC;      // 0x30
} ehrpwm_regs_t;

// Define pointers to eHRPWM1
#define EHRPWM1 ((ehrpwm_regs_t *) 0x01F02000)


void epwm1a_init(void);
void epwm1a_setDuty(uint16_t duty);

#endif /* PWM_H_ */
