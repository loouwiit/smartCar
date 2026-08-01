#pragma once

#include "filter.hpp"

// target
//	v
// PID	->	out
//	^		(|)
// actual	(<)

#define PID_DEBUG true

class PID
{
public:
	float target = 0.0f;

	float kp = 0.0f;
	float ki = 0.0f;
	float kd = 0.0f;

#if PID_DEBUG // for debug
	float portionP{};
	float portionI{};
	float portionD{};
#endif

	float integrationRange[2] = { -1.0f / 0.0f, +1.0f / 0.0f };
	float outputRange[2] = { -1.0f / 0.0f, +1.0f / 0.0f };

	Filter<> filterD{};

	void setTarget(float target);
	float update(float actual, float deltaT = 1.0f);
	float getOut();

	float operator()(float actual, float deltaT = 1.0f) { return update(actual, deltaT); };

	operator float() { return getOut(); }

	void clearIntegration();

private:
	float integration = 0.0f;
	float lastActual = 0.0f;

	float out = 0.0f;
};