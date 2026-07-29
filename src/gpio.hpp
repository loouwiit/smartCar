#pragma once
#include <ti/driverlib/dl_gpio.h>

class GPIO
{
public:
	GPIO_Regs* port = GPIOA;
	uint32_t pin = DL_GPIO_PIN_0;

	GPIO(GPIO_Regs* port, uint32_t pin) : port{ port }, pin{ pin } {};
	GPIO() = default;
	GPIO(GPIO&) = default;
	GPIO(GPIO&&) = default;
	GPIO& operator=(GPIO&) = default;
	GPIO& operator=(GPIO&&) = default;

	GPIO& clear()
	{
		port->DOUTCLR31_0 = pin;
		return *this;
	}

	GPIO& set()
	{
		port->DOUTSET31_0 = pin;
		return *this;
	}

	GPIO& toggle()
	{
		port->DOUTTGL31_0 = pin;
		return *this;
	}

	GPIO& set(bool flag)
	{
		if (flag) port->DOUTSET31_0 = pin;
		else port->DOUTCLR31_0 = pin;
		return *this;
	}

	GPIO& operator=(bool flag)
	{
		if (flag) port->DOUTSET31_0 = pin;
		else port->DOUTCLR31_0 = pin;
		return *this;
	};

	operator bool() const
	{
		return port->DIN31_0 & pin;
	};
};
