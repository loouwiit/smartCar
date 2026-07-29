#pragma once

#include <ti/driverlib/dl_timera.h>
#include <ti/driverlib/dl_timerg.h>

extern "C" void TIMA0_IRQHandler();
extern "C" void TIMA1_IRQHandler();
extern "C" void TIMG0_IRQHandler();
extern "C" void TIMG12_IRQHandler();
extern "C" void TIMG6_IRQHandler();
extern "C" void TIMG7_IRQHandler();
extern "C" void TIMG8_IRQHandler();

class Timer
{
public:
	using TimerInstance = GPTIMER_Regs*;
	using TimerCcIndex = DL_TIMER_CC_INDEX;

	class TimerId
	{
	public:
		unsigned char id = None;

		TimerId(unsigned char id = None) : id{ id } {}
		TimerId& operator=(TimerId&) = default;
		TimerId(TimerId&) = default;

		TimerId(const TimerId& copy) { id = copy.id; }
		TimerId& operator=(const TimerId& copy) { id = copy.id; return *this; }

		TimerId(TimerInstance inctance)
		{
			switch ((unsigned int)inctance)
			{
			case TIMA0_BASE: id = TimerA0; break;
			case TIMA1_BASE: id = TimerA1; break;
			case TIMG0_BASE: id = TimerG0; break;
			case TIMG12_BASE: id = TimerG12; break;
			case TIMG6_BASE: id = TimerG6; break;
			case TIMG7_BASE: id = TimerG7; break;
			case TIMG8_BASE: id = TimerG8; break;
			default: id = None; break;
			}
		};

		operator unsigned char() { return id; }

		constexpr static unsigned char None = -1;
		constexpr static unsigned char TimerA0 = 0;
		constexpr static unsigned char TimerA1 = 1;
		constexpr static unsigned char TimerG0 = 2;
		constexpr static unsigned char TimerG12 = 3;
		constexpr static unsigned char TimerG6 = 4;
		constexpr static unsigned char TimerG7 = 5;
		constexpr static unsigned char TimerG8 = 6;
		constexpr static unsigned char Count = 7;
	};

	using TimerEvent = DL_TIMER_IIDX;
	using TimerCallback_t = void(*)(TimerEvent event);

	Timer(TimerInstance timer) : timer{ timer } {}
	Timer() = default;
	Timer(Timer&) = default;
	Timer& operator=(Timer&) = default;

	void start();
	void stop();

	static void setCallbackFunction(TimerId id, TimerCallback_t function);

	void setLoad(uint32_t value);

	void setCount(uint32_t value);
	uint32_t getCount();

	static uint32_t getCc(TimerInstance timer, TimerCcIndex ccIndex);
	uint32_t getCc(TimerCcIndex ccIndex);
	void setCc(TimerCcIndex ccIndex, uint32_t value);

protected:
	friend void TIMA0_IRQHandler();
	friend void TIMA1_IRQHandler();
	friend void TIMG0_IRQHandler();
	friend void TIMG12_IRQHandler();
	friend void TIMG6_IRQHandler();
	friend void TIMG7_IRQHandler();
	friend void TIMG8_IRQHandler();

	static void emptyInterruptCallback(TimerEvent event) {};

	static TimerCallback_t interruptCallback[TimerId::Count];

protected:
	static volatile uint32_t& getCcRegister(TimerInstance timer, TimerCcIndex ccIndex);
	volatile uint32_t& getCcRegister(TimerCcIndex ccIndex);

	TimerInstance timer = nullptr;
};
