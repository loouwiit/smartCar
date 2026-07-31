#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "serve.hpp"
#include "fpid.hpp"
#include <cstring>

extern UART uart;
extern UART uartData;

extern Serve serve;
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

	serveFpid.kp = 3;
	serveFpid.kd = 50;

	serveFpid.setTarget(70);

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
	}
}

static void balanceRecieve(char* recieve)
{
	if (recieve[0] == '!')
	{
		dealRecieve(recieve + 1);
		return;
	}
	if (recieve[0] < '0' || '9' < recieve[0]) return;

	float position = atof(recieve);

	txSize = sprintf(txBuffer, "%f, %f %f %f\n", position, serveFpid.portionP, serveFpid.portionI, serveFpid.portionD);
	uart.transit(txBuffer, txSize);

	auto nowTime = xTaskGetTickCount();

	serveFpid.update(position, ((float)nowTime - (float)lastFpidTime));
	if (abs(position - serveFpid.getTarget()) >= 5)
		serve[Serve::MixNumber::BalanceFeed] = serveFpid.getOut();

	lastFpidTime = nowTime;
}
