#pragma once
#include <ti/driverlib/dl_timera.h>
#include <ti/driverlib/dl_timerg.h>

class PWM
{
public:
	using TimerCcIndex = DL_TIMER_CC_INDEX;
	using TimerInstance = GPTIMER_Regs*;

	PWM(TimerInstance timer, TimerCcIndex ccIndex) { ccRegister = &timer->COUNTERREGS.CC_01[0]; ccRegister += (uint32_t)ccIndex; }

	PWM() = default;
	PWM(PWM&) = default;
	PWM& operator=(PWM&) = default;

	PWM& operator=(unsigned short duty) { *ccRegister = duty; return *this; }
	operator uint32_t() { return *ccRegister; }
	PWM& operator+= (int x) { *ccRegister += x; return*this; }
	PWM& operator-= (int x) { *ccRegister -= x; return*this; }

private:
	volatile uint32_t* ccRegister = nullptr;
};
