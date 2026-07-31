#include "ti_msp_dl_config.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include <cstring>

#include "gpio.hpp"
#include "fpid.hpp"
#include "motor.hpp"
#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "stringCompare.hpp"
#include "mixer.hpp"
#include "graySensor.hpp"
#include "servo.hpp"
#include "mutex.hpp"

extern UART uart;
extern volatile float captureSpeeds[2];
extern volatile int moveCount[2];
extern FPID fpid[2];
extern FPID serveFpid[2];
extern Motor motor[2];
extern Mixer<float, MixNumber::Count> mixer[2];
extern Script<> script[2];
extern Servo servo;
extern GraySensor graySensor;
extern volatile bool grayEnable;

extern TickType_t oledTimeStart;
extern TickType_t oledTimeStop;

extern float pitch, roll, yaw;

constexpr int txBufferSize = 64;
static char txBuffer[txBufferSize] = "";
static int txSize = 0;

constexpr int rxBufferSize = 256;
static char rxBuffer[rxBufferSize]{};
static char* rxBufferSplit[64]{};
static unsigned char rxTokenLength[64]{};
static unsigned char rxSplitSize = 0;
static int rxSize = 0;
static int rxTotolSize = 0;

Mutex mutex{};
void dealRecieve(char* recieve);
unsigned char splitCommand(char* text, char** commands, unsigned char* lengths, char splitChar = ' ');

void uartThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("uart started\n", 13);

	while (true)
	{
		rxSize = uart.getReceiveCount();
		if (rxSize > 0)
		{
			// 接收数据
			rxSize = uart.getReceive(rxBuffer + rxTotolSize, rxSize);

			// 检查\n
			do
			{
				if (rxBuffer[rxTotolSize] == '\n' ||
					rxBuffer[rxTotolSize] == '\r')
				{
					// echo
					// while (uart.isTransiting())
					// 	vTaskDelay(1);
					// uart.transit(rxBuffer, rxTotolSize + 1);

					// deal
					rxBuffer[rxTotolSize] = '\0';
					dealRecieve(rxBuffer);
					vTaskDelay(1);

					// XX0AB
					rxTotolSize++; // '\0'
					rxSize--;
					for (int i = 0; i < rxSize;i++)
						rxBuffer[i] = rxBuffer[rxTotolSize + i];

					rxTotolSize = 0;
					continue;
				}
				rxTotolSize++;
				rxSize--;
			} while (rxSize > 0);
		}

		// transmit

		if (uart.isTransiting())
			vTaskDelay(1);

		txSize = 0;
		// txSize = sprintf(txBuffer, "%.1f->%.1f %.1f->%.1f\n", fpid[0].getOut(), fpid[0].getTarget(), fpid[1].getOut(), fpid[1].getTarget());
		// txSize = sprintf(txBuffer, "%.1f %.1f %.1f\t\t%.1f %.1f %.1f\n", mixer[0][0], mixer[0][1], mixer[0][2], mixer[1][0], mixer[1][1], mixer[1][2]);
		// txSize = sprintf(txBuffer, "%d %d %d %d %d %d %d %d\n", (bool)graySensor[0], (bool)graySensor[1], (bool)graySensor[2], (bool)graySensor[3], (bool)graySensor[4], (bool)graySensor[5], (bool)graySensor[6], (bool)graySensor[7]);
		// txSize = sprintf(txBuffer, "%.7f\n", (float)(moveCount[0] - moveCount[1]) / RoundCount);
		// txSize = sprintf(txBuffer, "speed %.2f %.2f %.2f %.2f\n",captureSpeeds[0], captureSpeeds[1], fpid[0].getOut(), fpid[1].getOut());
		// txSize = sprintf(txBuffer, "%4d %4d %4d %4d %4d %4d %4d %4d\n", (int)graySensor.getAdcValue(0), (int)graySensor.getAdcValue(1), (int)graySensor.getAdcValue(2), (int)graySensor.getAdcValue(3), (int)graySensor.getAdcValue(4), (int)graySensor.getAdcValue(5), (int)graySensor.getAdcValue(6), (int)graySensor.getAdcValue(7));

		if (txSize > 0)
		{
			while (uart.isTransiting())
				vTaskDelay(1);
			uart.transit(txBuffer, txSize);
		}
		vTaskDelay(configTICK_RATE_HZ / 10);
	}
}

void dealRecieve(char* recieve)
{
	const char* errorMessage = nullptr;

	rxSplitSize = splitCommand(recieve, rxBufferSplit, rxTokenLength);

	if (rxBufferSplit[0][0] == 's' || stringCompare(rxBufferSplit[0], rxTokenLength[0], "script", 6))
	{
		if (rxSplitSize <= 2) return;
		int strength = atoi(rxBufferSplit[1]);
		int duration = atoi(rxBufferSplit[2]);

		int delay = 0;
		int delayCount = 0;
		int deltaCount = 0;

		if (rxSplitSize > 3)
			delay = atoi(rxBufferSplit[3]);

		if (rxSplitSize > 4)
			delayCount = atoi(rxBufferSplit[4]);

		if (rxSplitSize > 5)
			deltaCount = atoi(rxBufferSplit[5]);

		Lock lock{ mutex };

		int freeEntryIndex = servo.script.getFreeScriptEntryIndex();
		if (freeEntryIndex == -1)
		{
			errorMessage = "no free script entry!";
			goto sendError;
		}
		auto& freeEntry = servo.script[freeEntryIndex];
		freeEntry.strength = strength;
		freeEntry.duration = pdMS_TO_TICKS(duration);

		if (delay != 0)
			freeEntry.delay = delay;
		if (delayCount != 0)
			freeEntry.startCount = moveCount[0] + moveCount[1] + delayCount;
		if (deltaCount != 0)
			freeEntry.stopCount = freeEntry.startCount + deltaCount;
	}
	else if (rxBufferSplit[0][0] == 'm' || stringCompare(rxBufferSplit[0], rxTokenLength[0], "move", 4))
	{
		if (rxSplitSize <= 3) return;
		float speed = atof(rxBufferSplit[1]);
		float rotate = atof(rxBufferSplit[2]);
		int duration = atoi(rxBufferSplit[3]);

		int delayTime{};
		int delayCount{};
		int deltaCount{};

		if (rxSplitSize > 4)
			delayTime = atoi(rxBufferSplit[4]);

		if (rxSplitSize > 5)
			delayCount = atoi(rxBufferSplit[5]);

		if (rxSplitSize > 6)
			deltaCount = atoi(rxBufferSplit[6]);

		if (duration == 0 && deltaCount == 0)
		{
			errorMessage = "duration and deltaCount cannot be both 0!\n";
			goto sendError;
		}

		int scriptIndex[2]{};

		while (true)
		{
			{
				Lock lock{ mutex };

				scriptIndex[0] = script[0].getFreeScriptEntryIndex();
				scriptIndex[1] = script[1].getFreeScriptEntryIndex();
				if (scriptIndex[0] != -1 && scriptIndex[1] != -1)
				{
					Script<>::ScriptEntry* scriptEntry[2]{ &script[0][scriptIndex[0]], &script[1][scriptIndex[1]] };

					for (auto& i : scriptEntry)
					{
						i->duration = duration == 0 ? portMAX_DELAY : pdMS_TO_TICKS(duration);

						if (delayTime != 0)
							i->delay = delayTime;
						if (delayCount != 0)
							i->startCount = moveCount[0] + moveCount[1] + delayCount;
						if (deltaCount != 0)
							i->stopCount = i->startCount + deltaCount;
					}

					float speedLeft = speed - rotate;
					float speedRight = speed + rotate;
					scriptEntry[0]->strength = speedLeft;
					scriptEntry[1]->strength = speedRight;
					break;
				}
			}

			while (uart.isTransiting())
				vTaskDelay(1);
			uart.transit("no free script entry!\n", 22);
			vTaskDelay(1);
		}
	}
	else if (stringCompare(rxBufferSplit[0], rxTokenLength[0], "track", 5))
	{
		if (rxSplitSize <= 1) return;
		Lock lock{ mutex };
		if (rxBufferSplit[1][0] == 'o' && rxBufferSplit[1][1] == 'n')
		{
			grayEnable = true;

			mixer[0].enable(MixNumber::GraySensor);
			mixer[1].enable(MixNumber::GraySensor);
		}
		else if (rxBufferSplit[1][0] == 'o' && rxBufferSplit[1][1] == 'f' && rxBufferSplit[1][2] == 'f')
		{
			grayEnable = false;

			mixer[0].disable(MixNumber::GraySensor);
			mixer[1].disable(MixNumber::GraySensor);
		}
	}
	else if (stringCompare(rxBufferSplit[0], rxTokenLength[0], "time", 4))
	{
		TickType_t timeOffset = 0;
		if (rxSplitSize > 1)
			timeOffset = atoi(rxBufferSplit[1]);

		Lock lock{ mutex };
		oledTimeStart = xTaskGetTickCount() + timeOffset;
		oledTimeStop = portMAX_DELAY;
	}
	else if (rxBufferSplit[0][0] == '+')
	{
		int delta = 10;
		Lock lock{ mutex };
		int target = servo[Servo::MixNumber::Uart];
		if (rxSplitSize > 1)
			delta = atoi(rxBufferSplit[1]);
		else delta = atoi(rxBufferSplit[0] + 1);
		if (delta == 0) delta = 100;
		target += delta;
		servo[Servo::MixNumber::Uart] = target;
	}
	else if (rxBufferSplit[0][0] == '-')
	{
		int delta = 10;
		Lock lock{ mutex };
		int target = servo[Servo::MixNumber::Uart];
		if (rxSplitSize > 1)
			delta = atoi(rxBufferSplit[1]);
		else delta = atoi(rxBufferSplit[0] + 1);
		if (delta == 0) delta = 100;
		target -= delta;
		servo[Servo::MixNumber::Uart] = target;
	}
	else if (rxBufferSplit[0][0] == '=')
	{
		Lock lock{ mutex };
		int target{};
		if (rxSplitSize > 1)
			target = atoi(rxBufferSplit[1]);
		else target = atoi(rxBufferSplit[0] + 1);
		if (target == 0)
			target = Servo::StandardBalancePoint;
		servo[Servo::MixNumber::Uart] = target;
	}
	else if (stringCompare(rxBufferSplit[0], rxTokenLength[0], "pid", 3))
	{
		if (rxSplitSize < 4) return;

		float kp = atof(rxBufferSplit[1]);
		float ki = atof(rxBufferSplit[2]);
		float kd = atof(rxBufferSplit[3]);

		Lock lock{ mutex };

		for (int i = 0; i < 2; i++)
		{
			serveFpid[i].kp = kp;
			serveFpid[i].ki = ki;
			serveFpid[i].kd = kd;

			serveFpid[i].integrationRange[0] = serveFpid[i].pidOutputRange[0] / serveFpid[i].ki;
			serveFpid[i].integrationRange[1] = serveFpid[i].pidOutputRange[1] / serveFpid[i].ki;
			serveFpid[i].clearIntegration();
		}
	}
	else if (stringCompare(rxBufferSplit[0], rxTokenLength[0], "target", 6))
	{
		if (rxSplitSize < 3) return;

		int index = atoi(rxBufferSplit[1]);
		if (index >= 2) return;

		float speed = atof(rxBufferSplit[2]);

		Lock lock{ mutex };

		mixer[index][MixNumber::Uart] = speed;

		if (speed == 0)
			fpid[index].clearIntegration();
	}
	else if (prefixCompare(rxBufferSplit[0], rxTokenLength[0], "close", 5))
	{
		Lock lock{ mutex };

		for (int i = 0; i < 2; i++)

		{
			fpid[i].kp = 0;
			fpid[i].ki = 0;
			fpid[i].kd = 0;

			fpid[i].integrationRange[0] = fpid[i].pidOutputRange[0] / fpid[i].ki;
			fpid[i].integrationRange[1] = fpid[i].pidOutputRange[1] / fpid[i].ki;

			fpid[i].clearIntegration();
			fpid[i].setTarget(0);;
		}
	}

sendError:
	if (errorMessage)
	{
		while (uart.isTransiting())
			vTaskDelay(1);
		uart.transit(errorMessage, strlen(errorMessage));
	}
}

unsigned char splitCommand(char* text, char** commands, unsigned char* lengths, char splitChar)
{
	unsigned char index = 0;
	commands[index] = text;
	index++;
	unsigned char length = 0;
	for (; *text != '\0'; text++)
	{
		if (*text == splitChar)
		{
			*text = '\0';
			lengths[index - 1] = length;
			text++;
			commands[index] = text;
			index++;
			length = 0;
		}
		else
			length++;
	}
	lengths[index - 1] = length;

	return index;
}
