#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "serve.hpp"

extern UART uart;
extern UART uartData;

constexpr int txBufferSize = 64;
static char txBuffer[txBufferSize] = "";
static int txSize = 0;

constexpr int rxBufferSize = 256;
static char rxBuffer[rxBufferSize]{};
static int rxSize = 0;
static int rxTotolSize = 0;

static void dealRecieve(char* recieve);

void balanceThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("balanceThread started\n", 22);

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
	}
}

static void dealRecieve(char* recieve)
{
	auto data = atof(recieve);

	txSize = sprintf(txBuffer, "data:%f\n", data);
	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit(txBuffer, txSize);
}
