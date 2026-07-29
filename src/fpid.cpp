#include "fpid.hpp"

void FPID::setPredictFunction(Function_t function)
{
	predictFunction = function;
	predictOut = predictFunction(target, this->param);
	fpidOut = PID::getOut() + predictOut;
	detectOutRange();
}

void FPID::setPredictFunction(Function_t function, Param_t param)
{
	predictFunction = function;
	predictOut = predictFunction(target, param);
	fpidOut = PID::getOut() + predictOut;
	detectOutRange();
}

void FPID::setPredictParam(Param_t param)
{
	this->param = param;
}

float FPID::getTarget()
{
	return target;
}

void FPID::setTarget(float target)
{
	PID::setTarget(target);
	predictOut = predictFunction(target, this->param);
	fpidOut = PID::getOut() + predictOut;
	detectOutRange();
}

void FPID::setTarget(float target, Param_t param)
{
	PID::setTarget(target);
	predictOut = predictFunction(target, param);
	fpidOut = PID::getOut() + predictOut;
	detectOutRange();
}

float FPID::update(float actual, float deltaT)
{
	fpidOut = PID::update(actual, deltaT) + predictOut;
	detectOutRange();
	return fpidOut;
}

float FPID::getOut()
{
	return fpidOut;
}

void FPID::detectOutRange()
{
	if (fpidOut < outputRange[0])
		fpidOut = outputRange[0];
	if (fpidOut > outputRange[1])
		fpidOut = outputRange[1];
}
