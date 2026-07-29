#include "motor.hpp"

Motor& Motor::operator=(int speed)
{
	setSpeed(speed);
	return *this;
}

void Motor::setSpeed(int speed)
{
	if (speed > 0)
	{
		channel[0].set();
		channel[1].clear();
		pwm = speed;
	}
	else if (speed < 0)
	{
		channel[0].clear();
		channel[1].set();
		pwm = -speed;
	}
	else
	{
		channel[0].clear();
		channel[1].clear();
		pwm = 0;
	}
}
