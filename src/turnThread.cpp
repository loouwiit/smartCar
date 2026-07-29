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

extern UART uart;

extern unsigned getTime();
extern float captureSpeeds[2];
extern int moveCount[2];

extern FPID fpid[2];
extern Mixer<float, MixNumber::Count> mixer[2];

int turnContorl = 0;
int leftTurnCount = -550;
unsigned turnRoundCount = 0;

constexpr static int turnDeadZone = 20;
constexpr static TickType_t stopTime = 4000;

static int turnCountOffset = 0;

void strightKeepThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("turn started\n", 13);

	while (true)
	{
		while (turnContorl == 0 || turnRoundCount == 0)
		{
			vTaskDelay(100);
			continue;
		}

		mixer[0].disable(MixNumber::Key);
		mixer[1].disable(MixNumber::Key);
		mixer[0].disable(MixNumber::GraySensor);
		mixer[1].disable(MixNumber::GraySensor);
		turnCountOffset = moveCount[0] - moveCount[1];

		while (turnContorl != 0)
		{
			auto delta = moveCount[0] - moveCount[1] - turnCountOffset - turnContorl;
			if (delta > turnDeadZone)
			{
				mixer[0][MixNumber::Turn] = -1.0f;
				mixer[1][MixNumber::Turn] = +1.0f;
			}
			else if (delta < -turnDeadZone)
			{
				mixer[0][MixNumber::Turn] = +1.0f;
				mixer[1][MixNumber::Turn] = -1.0f;
			}
			else
			{
				mixer[0][MixNumber::Turn] = 0;
				mixer[1][MixNumber::Turn] = 0;

				turnContorl = 0;
			}

			mixer[0].mix();
			mixer[1].mix();

			vTaskDelay(10);
		}

		vTaskDelay(100);
		mixer[0].enable(MixNumber::GraySensor);
		mixer[1].enable(MixNumber::GraySensor);
		vTaskDelay(100);
		mixer[0].enable(MixNumber::Key);
		mixer[1].enable(MixNumber::Key);

		turnRoundCount--;
		if (turnRoundCount == 0)
		{
			vTaskDelay(stopTime);
			void stop();
			stop();
		}
	}
}
