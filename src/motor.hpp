#pragma once

#include "gpio.hpp"
#include "pwm.hpp"

class Motor
{
public:
	Motor& operator=(int speed);
	void setSpeed(int speed);

	GPIO channel[2];
	PWM pwm;
};
