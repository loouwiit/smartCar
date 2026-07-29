#include "timer.hpp"

Timer::TimerCallback_t Timer::interruptCallback[TimerId::Count]{ emptyInterruptCallback,emptyInterruptCallback,emptyInterruptCallback,emptyInterruptCallback,emptyInterruptCallback,emptyInterruptCallback,emptyInterruptCallback };

extern "C" void TIMA0_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMA0);
	Timer::interruptCallback[Timer::TimerId::TimerA0](event);
}

extern "C" void TIMA1_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMA1);
	Timer::interruptCallback[Timer::TimerId::TimerA1](event);
}

extern "C" void TIMG0_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMG0);
	Timer::interruptCallback[Timer::TimerId::TimerG0](event);
}

extern "C" void TIMG12_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMG12);
	Timer::interruptCallback[Timer::TimerId::TimerG12](event);
}

extern "C" void TIMG6_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMG6);
	Timer::interruptCallback[Timer::TimerId::TimerG6](event);
}

extern "C" void TIMG7_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMG7);
	Timer::interruptCallback[Timer::TimerId::TimerG7](event);
}

extern "C" void TIMG8_IRQHandler()
{
	DL_TIMER_IIDX event = DL_Timer_getPendingInterrupt(TIMG8);
	Timer::interruptCallback[Timer::TimerId::TimerG8](event);
}

void Timer::start()
{
	DL_Timer_startCounter(timer);
}

void Timer::stop()
{
	DL_Timer_stopCounter(timer);
}

void Timer::setCallbackFunction(TimerId id, TimerCallback_t function)
{
	if (function == nullptr)
		function = emptyInterruptCallback;

	interruptCallback[id] = function;
}

void Timer::setLoad(uint32_t value)
{
	timer->COUNTERREGS.LOAD = value;
}

void Timer::setCount(uint32_t value)
{
	timer->COUNTERREGS.CTR = value;
}

uint32_t Timer::getCount()
{
	return timer->COUNTERREGS.CTR;
}

void Timer::setCc(TimerCcIndex ccIndex, uint32_t value)
{
	getCcRegister(ccIndex) = value;
}

uint32_t Timer::getCc(TimerInstance timer, TimerCcIndex ccIndex)
{
	return getCcRegister(timer, ccIndex);
}

uint32_t Timer::getCc(TimerCcIndex ccIndex)
{
	return getCcRegister(ccIndex);
}

volatile uint32_t& Timer::getCcRegister(TimerInstance timer, TimerCcIndex ccIndex)
{
	volatile uint32_t* ccRegister = &timer->COUNTERREGS.CC_01[0];
	ccRegister += (uint32_t)ccIndex;
	return *ccRegister;
}

volatile uint32_t& Timer::getCcRegister(TimerCcIndex ccIndex)
{
	volatile uint32_t* ccRegister = &timer->COUNTERREGS.CC_01[0];
	ccRegister += (uint32_t)ccIndex;
	return *ccRegister;
}
