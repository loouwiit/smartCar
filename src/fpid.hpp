#pragma once

#include "pid.hpp"

class FPID : protected PID
{
public:
	// predict
	using Param_t = void*;
	using Function_t = float (*)(float target, Param_t param);

	void setPredictFunction(Function_t function);
	void setPredictFunction(Function_t function, Param_t param);
	void setPredictParam(Param_t param);

	// pid
	using PID::kp;
	using PID::ki;
	using PID::kd;

	using PID::integrationRange;
	float(&pidOutputRange)[2] = PID::outputRange;

	float getTarget();
	void setTarget(float target);
	void setTarget(float target, Param_t param);
	float update(float actual, float deltaT = 1.0f);
	float getOut();

	float operator()(float actual, float deltaT = 1.0f) { return update(actual, deltaT); };

	operator float() { return getOut(); }

	using PID::clearIntegration;

	// fpid
	float outputRange[2] = { -1.0f / 0.0f, +1.0f / 0.0f };

private:
	void detectOutRange();

	static float emptyFunction(float, Param_t) { return 0.0f; };

	Function_t predictFunction = emptyFunction;
	Param_t param = nullptr;
	float predictOut = 0.0f;
	float fpidOut = 0.0f;
};
