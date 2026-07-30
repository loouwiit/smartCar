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
#include "serve.hpp"

extern UART uart;
extern float captureSpeeds[2];
extern int moveCount[2];
extern FPID fpid[2];
extern Motor motor[2];
extern Mixer<float, MixNumber::Count> mixer[2];
extern Script<float, 10> script[2];
extern Serve serve;
extern GraySensor graySensor;

extern int RoundCount;

extern float pitch, roll, yaw;

constexpr int txBufferSize = 64;
static char txBuffer[txBufferSize] = "";
static int txSize = 0;

constexpr int rxBufferSize = 256;
static char rxBuffer[rxBufferSize]{};
static char* rxBufferSplit[64]{};
static unsigned char rxSplitSize = 0;
static int rxSize = 0;
static int rxTotolSize = 0;

void dealRecieve(char* recieve);
unsigned char splitCommand(char* text, char** commands, char splitChar = ' ');

void uartThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("uart started\n", 14);

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
		// txSize = sprintf(txBuffer, "%.1f %.1f %.1f %.1f\t\t%.1f %.1f %.1f %.1f %d\n", mixer[0][0], mixer[0][1], mixer[0][2], mixer[0][3], mixer[1][0], mixer[1][1], mixer[1][2], mixer[1][3], turnContorl);
		// txSize = sprintf(txBuffer, "%d %d %d %d %d %d %d %d\n", (bool)graySensor[0], (bool)graySensor[1], (bool)graySensor[2], (bool)graySensor[3], (bool)graySensor[4], (bool)graySensor[5], (bool)graySensor[6], (bool)graySensor[7]);
		// txSize = sprintf(txBuffer, "%.7f\n", (float)(moveCount[0] - moveCount[1]) / RoundCount);
		// txSize = sprintf(txBuffer, "speed %.2f %.2f %.2f %.2f\n",captureSpeeds[0], captureSpeeds[1], fpid[0].getOut(), fpid[1].getOut());
		// txSize = sprintf(txBuffer, "%4d %4d %4d %4d %4d %4d %4d %4d\n", (int)graySensor.getAdcValue(0), (int)graySensor.getAdcValue(1), (int)graySensor.getAdcValue(2), (int)graySensor.getAdcValue(3), (int)graySensor.getAdcValue(4), (int)graySensor.getAdcValue(5), (int)graySensor.getAdcValue(6), (int)graySensor.getAdcValue(7));

		while (uart.isTransiting())
			vTaskDelay(1);
		uart.transit(txBuffer, txSize);
		vTaskDelay(configTICK_RATE_HZ / 10);
	}
}

void dealRecieve(char* recieve)
{
	rxSplitSize = splitCommand(recieve, rxBufferSplit);

	if (rxBufferSplit[0][0] == '+')
	{
		int delta = 10;
		int target = serve[Serve::MixNumber::Uart];
		if (rxSplitSize > 1)
			delta = atoi(rxBufferSplit[1]);
		else delta = atoi(rxBufferSplit[0] + 1);
		if (delta == 0) delta = 100;
		target += delta;
		serve[Serve::MixNumber::Uart] = target;

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "added %d to %d\n", delta, target);
		uart.transit(txBuffer, txSize);
	}
	else if (rxBufferSplit[0][0] == '-')
	{
		int delta = 10;
		int target = serve[Serve::MixNumber::Uart];
		if (rxSplitSize > 1)
			delta = atoi(rxBufferSplit[1]);
		else delta = atoi(rxBufferSplit[0] + 1);
		if (delta == 0) delta = 100;
		target -= delta;
		serve[Serve::MixNumber::Uart] = target;

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "subed %d to %d\n", delta, target);
		uart.transit(txBuffer, txSize);
		while (uart.isTransiting())
			vTaskDelay(1);
	}
	else if (rxBufferSplit[0][0] == '=')
	{
		int target{};
		if (rxSplitSize > 1)
			target = atoi(rxBufferSplit[1]);
		else target = atoi(rxBufferSplit[0] + 1);
		if (target == 0)
			target = Serve::StandardBalancePoint;

		serve[Serve::MixNumber::Uart] = target;

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "set to %d\n", target);
		uart.transit(txBuffer, txSize);
	}
	else if (rxBufferSplit[0][0] == 's' || stringCompare(rxBufferSplit[0], rxBufferSplit[1] - rxBufferSplit[0] - 1, "script", 6))
	{
		if (rxSplitSize <= 2) return;
		int strength = atoi(rxBufferSplit[1]);
		int duration = atoi(rxBufferSplit[2]);
		int offset = 0;
		if (rxSplitSize > 3)
			offset = atoi(rxBufferSplit[3]);

		int freeEntryIndex = serve.script.getFreeScriptEntryIndex();
		if (freeEntryIndex == -1)
		{
			while (uart.isTransiting())
				vTaskDelay(1);
			uart.transit("no free script entry!", 21);
			return;
		}
		auto& freeEntry = serve.script[freeEntryIndex];
		freeEntry.startTime = xTaskGetTickCount() + offset;
		freeEntry.expireTime = freeEntry.startTime + pdMS_TO_TICKS(duration);
		freeEntry.strength = strength;
	}
	else if (stringCompare(rxBufferSplit[0], rxBufferSplit[1] - rxBufferSplit[0] - 1, "pid", 3))
	{
		if (rxSplitSize < 4) return;

		float kp = atof(rxBufferSplit[1]);
		float ki = atof(rxBufferSplit[2]);
		float kd = atof(rxBufferSplit[3]);

		for (int i = 0; i < 2; i++)
		{
			fpid[i].kp = kp;
			fpid[i].ki = ki;
			fpid[i].kd = kd;

			fpid[i].integrationRange[0] = fpid[i].pidOutputRange[0] / fpid[i].ki;
			fpid[i].integrationRange[1] = fpid[i].pidOutputRange[1] / fpid[i].ki;
			fpid[i].clearIntegration();
		}
	}
	else if (stringCompare(rxBufferSplit[0], rxBufferSplit[1] - rxBufferSplit[0] - 1, "target", 6))
	{
		if (rxSplitSize < 3) return;

		int index = atoi(rxBufferSplit[1]);
		if (index >= 2) return;

		float speed = atof(rxBufferSplit[2]);

		mixer[index][MixNumber::Uart] = speed;

		if (speed == 0)
			fpid[index].clearIntegration();
	}
	else if (rxBufferSplit[0][0] == 'm' || stringCompare(rxBufferSplit[0], rxBufferSplit[1] - rxBufferSplit[0] - 1, "move", 4))
	{
		if (rxSplitSize < 3) return;
		float speed = atof(rxBufferSplit[1]);
		float rotate = atof(rxBufferSplit[2]);

		int scriptIndex[2]{};

		while (true)
		{
			scriptIndex[0] = script[0].getFreeScriptEntryIndex();
			scriptIndex[1] = script[1].getFreeScriptEntryIndex();
			if (scriptIndex[0] != -1 && scriptIndex[1] != -1)
				break;
			while (uart.isTransiting())
				vTaskDelay(1);
			uart.transit("no free script entry!\n", 22);
			vTaskDelay(1);
		}

		Script<float, 10>::ScriptEntry* scriptEntry[2]{ &script[0][scriptIndex[0]], &script[1][scriptIndex[1]] };

		int delay{};
		int duration{};

		if (rxSplitSize > 3)
			duration = atoi(rxBufferSplit[3]);

		if (rxSplitSize > 4)
			delay = atoi(rxBufferSplit[4]);

		auto nowTime = xTaskGetTickCount();
		for (auto& i : scriptEntry)
		{
			i->startTime = nowTime + delay;
			i->expireTime = i->startTime + duration;
		}

		float speedLeft = speed - rotate;
		float speedRight = speed + rotate;
		scriptEntry[0]->strength = speedLeft;
		scriptEntry[1]->strength = speedRight;
	}
	else if (prefixCompare(rxBufferSplit[0], strlen(rxBufferSplit[0]), "close", 5))
	{
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
}

unsigned char splitCommand(char* text, char** commands, char splitChar)
{
	unsigned char index = 0;
	commands[index] = text;
	index++;
	for (;*text != '\0';text++)
	{
		if (*text == splitChar)
		{
			*text = '\0';
			text++;
			commands[index] = text;
			index++;
		}
	}

	return index;
}
