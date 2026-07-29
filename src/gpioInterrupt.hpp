#pragma once

#include "gpio.hpp"

extern "C" void GROUP1_IRQHandler();

class GpioInterrupt
{
public:
	constexpr static unsigned GpioPortCount = 2;
	constexpr static unsigned GpioPinCount = 32;
	using CallbackFunction_t = void (*)();

	static void init()
	{
		clear();
		NVIC_EnableIRQ(GPIOA_INT_IRQn);
		NVIC_EnableIRQ(GPIOB_INT_IRQn);
	}

	static void clear()
	{
		lastGpioState[0] = GPIOA->DIN31_0;
		lastGpioState[1] = GPIOB->DIN31_0;
	}

	static void setCallback(GPIO_Regs* portReg, uint32_t pin, CallbackFunction_t raise, CallbackFunction_t down)
	{
		unsigned char portNum = 0;
		unsigned char pinNum = 0;
		switch ((int)portReg)
		{
		case (int)GPIOA_BASE: portNum = 0; break;
		case (int)GPIOB_BASE: portNum = 1; break;
		default: break;
		}

		while (pin != 0)
		{
			if (pin & 0x1) break;
			pin >>= 1;
			pinNum++;
		}

		setCallback(portNum, pinNum, raise, down);
	}

	static void setCallback(unsigned char portNum, unsigned char pinNum, CallbackFunction_t raise, CallbackFunction_t down)
	{
		if (raise == nullptr && down == nullptr)
			enableMask[portNum] &= ~(1 << pinNum);
		else
			enableMask[portNum] |= (1 << pinNum);

		if (raise == nullptr)
			raise = emptyCallback;
		if (down == nullptr)
			down = emptyCallback;

		callbackFunction[portNum][pinNum][0] = raise;
		callbackFunction[portNum][pinNum][1] = down;
	}

private:
	static uint32_t lastGpioState[GpioPortCount];
	static uint32_t enableMask[GpioPortCount];
	static CallbackFunction_t callbackFunction[GpioPortCount][GpioPinCount + 1][2];

	static void emptyCallback() {}
	friend void GROUP1_IRQHandler();
};
