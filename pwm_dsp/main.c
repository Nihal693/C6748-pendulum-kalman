#include "pwm.h"
#include "evmomapl138.h"

#define EPWM1A_PINMUX               5
#define EPWM1A_BITFIELD             0x0000000F
#define EPWM1A_VAL                  0x00000002

interrupt void interrupt4(){

}

int main(void)
{
        // Step 1: Enable eHRPWM1 clock
        EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_PWM, PSC_ENABLE);

        // Step 2: Configure pinmux J15 pin 40 → eHRPWM1A
        EVMOMAPL138_pinmuxConfig(EPWM1A_PINMUX, EPWM1A_BITFIELD, EPWM1A_VAL);

        epwm1a_init();

        epwm1a_setDuty(5000);

        while(1){

        }

	return 0;
}
