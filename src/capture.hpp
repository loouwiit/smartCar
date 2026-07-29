#pragma once

#include "timer.hpp"

class Capture : protected Timer
{
public:
	using Timer::TimerId;
	using Timer::TimerInstance;
	using Timer::TimerCcIndex;
	using InterruptRequestNumber = IRQn_Type;

	Capture(TimerInstance timer, InterruptRequestNumber interruptRequestNumber);

	void start();
	void stop();

	using Timer::setCallbackFunction;

	using Timer::setLoad;
	using Timer::setCount;
	using Timer::getCount;

	uint32_t getCc(TimerCcIndex index);
	void setCc(TimerCcIndex index, uint32_t value);
	volatile uint32_t& operator[](TimerCcIndex index);

private:
	InterruptRequestNumber interruptRequestNumber{};
	TimerCcIndex ccIndex{};
	volatile uint32_t* ccRegisters = nullptr;
};
