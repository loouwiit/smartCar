#include "pid.hpp"

void PID::setTarget(float target)
{
	this->target = target;
	this->lastActual = target;
}

float PID::update(float actual, float deltaT)
{
	float error = target - actual;
	integration += error * deltaT;
	float delta = 0;
	if (deltaT != 0)
		delta = -(actual - lastActual) / deltaT;
	lastActual = actual;

	// 积分限幅
	if (integration < integrationRange[0])
		integration = integrationRange[0];
	if (integration > integrationRange[1])
		integration = integrationRange[1];

	float p = kp * error;
	float i = ki * integration;
	float d = kd * filterD(delta);

#if PID_DEBUG
	portionP = p;
	portionI = i;
	portionD = d;
#endif

	out = p + i + d;

	// 输出限幅
	if (out < outputRange[0])
		out = outputRange[0];
	if (out > outputRange[1])
		out = outputRange[1];

	return out;
}

float PID::getOut()
{
	return out;
}

void PID::clearIntegration()
{
	integration = 0.0f;
}
