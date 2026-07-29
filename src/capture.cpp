#include "capture.hpp"

Capture::Capture(TimerInstance timer, InterruptRequestNumber interruptRequestNumber) : Timer(timer), interruptRequestNumber{ interruptRequestNumber }
{
	ccRegisters = &Timer::getCcRegister(TimerCcIndex::DL_TIMER_CC_0_INDEX);
}

void Capture::start()
{
	NVIC_EnableIRQ(interruptRequestNumber);
	Timer::start();
}

void Capture::stop()
{
	NVIC_DisableIRQ(interruptRequestNumber);
	Timer::stop();
}

uint32_t Capture::getCc(TimerCcIndex index)
{
	return ccRegisters[(int)index];
}

void Capture::setCc(TimerCcIndex index, uint32_t value)
{
	ccRegisters[(int)index] = value;
}

volatile uint32_t& Capture::operator[](TimerCcIndex index)
{
	return ccRegisters[index];
}
