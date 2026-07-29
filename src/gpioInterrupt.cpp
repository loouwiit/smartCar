#include <ti/driverlib/m0p/dl_interrupt.h>
#include "gpioInterrupt.hpp"

uint32_t GpioInterrupt::lastGpioState[GpioPortCount]{};
uint32_t GpioInterrupt::enableMask[GpioPortCount]{};
GpioInterrupt::CallbackFunction_t GpioInterrupt::callbackFunction[GpioPortCount][GpioPinCount + 1][2]{GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback, GpioInterrupt::emptyCallback};

static uint32_t detectIoChange(GPIO_Regs *port, uint32_t &lastState, uint32_t mask)
{
	uint32_t nowState = port->DIN31_0;
	uint32_t different = lastState ^ nowState;
	different &= mask;
	lastState = nowState;
	return different;
}

extern "C" void GROUP1_IRQHandler()
{
	switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
	{
	case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
	{
		auto different = detectIoChange(GPIOA, GpioInterrupt::lastGpioState[0], GpioInterrupt::enableMask[0]);
		for (int i = 0; i < 32; i++)
		{
			if (different & (1 << i))
				GpioInterrupt::callbackFunction[0][i][(GpioInterrupt::lastGpioState[0] & (1 << i)) == 0]();
		}
		break;
	}
	case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
	{
		auto different = detectIoChange(GPIOB, GpioInterrupt::lastGpioState[1], GpioInterrupt::enableMask[1]);
		for (int i = 0; i < 32; i++)
		{
			if (different & (1 << i))
				GpioInterrupt::callbackFunction[1][i][(GpioInterrupt::lastGpioState[1] & (1 << i)) == 0]();
		}
		break;
	}
	default:
		break;
	}
}
