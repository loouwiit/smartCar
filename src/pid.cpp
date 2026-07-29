#include "pid.hpp"

void PID::setTarget(float target)
{
	this->target = target;
}

float PID::update(float actual, float deltaT)
{
	float error = target - actual;
	integration += error * deltaT;
	float delta = -(actual - lastActual) / deltaT;
	if (deltaT == 0) delta = 0;

	// 积分限幅
	if (integration < integrationRange[0])
		integration = integrationRange[0];
	if (integration > integrationRange[1])
		integration = integrationRange[1];

	float p = kp * error;
	float i = ki * integration;
	float d = kd * delta;

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
