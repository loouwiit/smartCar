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

extern UART uart;
extern float captureSpeeds[2];
extern int moveCount[2];
extern FPID fpid[2];
extern Motor motor[2];
extern Mixer<float, MotorMixNumber::Count> motorMixer[2];
extern Mixer<float, ServemoterMixNumber::Count> serveMixer;
extern GraySensor graySensor;

extern int turnContorl;
extern int leftTurnCount;
extern int RoundCount;

extern float pitch, roll, yaw;

constexpr int txBufferSize = 64;
static char txBuffer[txBufferSize] = "";
static int txSize = 0;

constexpr int rxBufferSize = 64;
static char rxBuffer[rxBufferSize]{};
static char* rxBufferSplit[10]{};
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
					while (uart.isTransiting())
						vTaskDelay(1);
					uart.transit(rxBuffer, rxTotolSize + 1);

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
		// txSize = sprintf(txBuffer, "%.1f %.1f %.1f %.1f\t\t%.1f %.1f %.1f %.1f %d\n", motorMixer[0][0], motorMixer[0][1], motorMixer[0][2], motorMixer[0][3], motorMixer[1][0], motorMixer[1][1], motorMixer[1][2], motorMixer[1][3], turnContorl);
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
		int target = serveMixer[ServemoterMixNumber::Uart];
		if (rxSplitSize > 1)
			delta = atoi(rxBufferSplit[1]);
		else delta = atoi(rxBufferSplit[0] + 1);
		if (delta == 0) delta = 100;
		target += delta;
		if (target >= 2500)
			target = 2500 - 1;
		serveMixer[ServemoterMixNumber::Uart] = target;

		vTaskDelay(pdMS_TO_TICKS(200));
		serveMixer[ServemoterMixNumber::Uart] = 2335;

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "added %d to %d\n", delta, target);
		uart.transit(txBuffer, txSize);
	}
	else if (rxBufferSplit[0][0] == '-')
	{
		int delta = 10;
		int target = serveMixer[ServemoterMixNumber::Uart];
		if (rxSplitSize > 1)
			delta = atoi(rxBufferSplit[1]);
		else delta = atoi(rxBufferSplit[0] + 1);
		if (delta == 0) delta = 100;
		target -= delta;
		if (target < 500)
			target = 500;
		serveMixer[ServemoterMixNumber::Uart] = target;

		vTaskDelay(pdMS_TO_TICKS(200));
		serveMixer[ServemoterMixNumber::Uart] = 2335;

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "added %d to %d\n", delta, target);
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
			target = 2335;

		if (target < 500)
			target = 500;
		if (target >= 2500)
			target = 2500 - 1;
		serveMixer[ServemoterMixNumber::Uart] = target;

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "set to %d\n", target);
		uart.transit(txBuffer, txSize);
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

		motorMixer[index][MotorMixNumber::Uart] = speed;
		motorMixer[index].mix();
		fpid[index].setTarget(motorMixer[index]);

		if (speed == 0)
			fpid[index].clearIntegration();
	}
	else if (stringCompare(rxBufferSplit[0], rxBufferSplit[1] - rxBufferSplit[0] - 1, "move", 4))
	{
		if (rxSplitSize < 3) return;
		float speed = atof(rxBufferSplit[1]);
		float rotate = atof(rxBufferSplit[2]);

		float speedLeft = speed - rotate;
		float speedRight = speed + rotate;

		motorMixer[0][MotorMixNumber::Uart] = speedLeft;
		motorMixer[0].mix();
		fpid[0].setTarget(motorMixer[0]);

		motorMixer[1][MotorMixNumber::Uart] = speedRight;
		motorMixer[1].mix();
		fpid[1].setTarget(motorMixer[1]);

		for (int i = 0; i < 2;i++)
			fpid[i].clearIntegration();
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
