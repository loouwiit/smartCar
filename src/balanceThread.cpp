#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "servo.hpp"
#include "fpid.hpp"
#include "mutex.hpp"
#include <cstring>

extern UART uart;
extern UART uartData;

constexpr static int DeadZone = 5;
extern Servo servo;
extern Mutex* mutex;
FPID serveFpid{};
static TickType_t lastFpidTime{};

constexpr int txBufferSize = 64;
static char txBuffer[txBufferSize] = "";
static int txSize = 0;

constexpr int rxBufferSize = 256;
static char rxBuffer[rxBufferSize]{};
static int rxSize = 0;
static int rxTotolSize = 0;

void dealRecieve(char* recieve);
static void balanceRecieve(char* recieve);

void balanceThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("balanceThread started\n", 22);

	serveFpid.outputRange[0] = -150;
	serveFpid.outputRange[1] = +150;

	serveFpid.kp = 7;
	serveFpid.kd = 5000;

	serveFpid.setTarget(120);

	while (true)
	{
		rxSize = uartData.getReceiveCount();
		if (rxSize > 0)
		{
			// 接收数据
			rxSize = uartData.getReceive(rxBuffer + rxTotolSize, rxSize);

			// 检查\n
			do
			{
				if (rxBuffer[rxTotolSize] == '\n' ||
					rxBuffer[rxTotolSize] == '\r')
				{
					// echo
					// rxBuffer[rxTotolSize] = '\n';
					// while (uart.isTransiting())
					// 	vTaskDelay(1);
					// uart.transit(rxBuffer, rxTotolSize + 1);

					// deal
					rxBuffer[rxTotolSize] = '\0';
					balanceRecieve(rxBuffer);
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
		vTaskDelay(1);
	}
}

static void balanceRecieve(char* recieve)
{
	if (recieve[0] == '!')
	{
		dealRecieve(recieve + 1);
		return;
	}
	if (recieve[0] < '0' || '9' < recieve[0])
	{
		uart.transit("?\n", 2);
		return;
	};

	float position = 250.0f - atof(recieve);

	float portionP, portionI, portionD;
	auto nowTime = xTaskGetTickCount();

	{
		Lock lock{ *mutex };
		serveFpid.update(position, ((float)nowTime - (float)lastFpidTime));
		if (abs(position - serveFpid.getTarget()) >= DeadZone)
			servo[Servo::MixNumber::BalanceFeed] = serveFpid.getOut();
		else servo[Servo::MixNumber::BalanceFeed] = 0;
		portionP = serveFpid.portionP;
		portionI = serveFpid.portionI;
		portionD = serveFpid.portionD;
		lastFpidTime = nowTime;
	}

	txSize = sprintf(txBuffer, "%f, %f %f %f\n", position, portionP, portionI, portionD);
	uart.transit(txBuffer, txSize);
}
