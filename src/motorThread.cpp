#include "ti_msp_dl_config.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "autoDeleteThread.hpp"
#include "capture.hpp"
#include "motor.hpp"
#include "fpid.hpp"
#include "uart.hpp"
#include "mixer.hpp"
#include "script.hpp"

extern UART uart;

extern unsigned getTime();
extern float captureSpeeds[2];
extern int moveCount[2];

Motor motor[2]{
	{{{Motor_A2_PORT, Motor_A2_PIN}, {Motor_A1_PORT, Motor_A1_PIN}}, {Motor_Pwm_INST, Timer::TimerCcIndex::DL_TIMER_CC_0_INDEX}},
	{{{Motor_B2_PORT, Motor_B2_PIN}, {Motor_B1_PORT, Motor_B1_PIN}}, {Motor_Pwm_INST, Timer::TimerCcIndex::DL_TIMER_CC_1_INDEX}} };
FPID fpid[2]{};
Mixer<float, MixNumber::Count> mixer[2]{};
Script<float, 10> script[2]{};

float predict(float target, void*)
{
	if (target == 0.0f)
		return 0.0f; // float ==, might false
	return 30.0f + target * 60.0f;
}

void motorThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	mixer[0].setMixCallback([]()
		{
			mixer[0][MixNumber::Script] = script[0].getScriptTotol(xTaskGetTickCount());
			fpid[0].setTarget(mixer[0]);
		});

	mixer[1].setMixCallback([]()
		{
			mixer[1][MixNumber::Script] = script[1].getScriptTotol(xTaskGetTickCount());
			fpid[1].setTarget(mixer[1]);
		});

	for (int i = 0; i < 2; i++)
	{
		fpid[i].setPredictFunction(predict);

		fpid[i].kp = 0;
		fpid[i].ki = 1e-3;
		fpid[i].kd = 0;

		fpid[i].pidOutputRange[0] = -1000;
		fpid[i].pidOutputRange[1] = +1000;

		fpid[i].outputRange[0] = -1000;
		fpid[i].outputRange[1] = +1000;

		fpid[i].integrationRange[0] = fpid[i].pidOutputRange[0] / fpid[i].ki;
		fpid[i].integrationRange[1] = fpid[i].pidOutputRange[1] / fpid[i].ki;
	}

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("motor started\n", 14);

	unsigned nowTime = getTime();
	unsigned lastPidTime = nowTime;

	while (true)
	{
		nowTime = getTime();

		for (int i = 0; i < 2; i++)
		{
			if (captureSpeeds[i] == 0.0f && fpid[i].getTarget() == 0.0f) fpid[i].clearIntegration();

			mixer[i].mix();
			fpid[i].update(captureSpeeds[i], nowTime - lastPidTime);
			motor[i].setSpeed((int)fpid[i]);
		}
		lastPidTime = nowTime;

		vTaskDelay(10);
	}
}
