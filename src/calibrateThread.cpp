#include "ti_msp_dl_config.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart.hpp"
#include "graySensor.hpp"
#include "autoDeleteThread.hpp"

#include "cstdio"

extern UART uart;
extern GraySensor graySensor;

static GPIO key{ KEY_PORT, KEY_KEY_1_PIN };
static GPIO led{ LED_PORT,LED_G_PIN };

void calibrateThread(void*)
{
	AutoDeleteThread autoDeleteThread{};
	int low[8]{}, high[8]{}, threshold[8]{};
	char txBuffer[128]{};
	int txSize{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("calibrateThread started\n", 24);

	while (key == true) // release
	{
		led.toggle();
		vTaskDelay(100);
	}

	while (key == false) // press
		vTaskDelay(100);

	vTaskDelay(2000);
	for (int i = 0; i < 8; i++)
		low[i] = graySensor.getAdcValue(i);

	txSize = sprintf(txBuffer, "low: %d %d %d %d %d %d %d %d\n", low[0], low[1], low[2], low[3], low[4], low[5], low[6], low[7]);

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit(txBuffer, txSize);

	while (key == true) // release
	{
		led.toggle();
		vTaskDelay(200);
	}

	while (key == false) // press
		vTaskDelay(1);

	vTaskDelay(2000);
	for (int i = 0; i < 8; i++)
		high[i] = graySensor.getAdcValue(i);

	while (uart.isTransiting())
		vTaskDelay(1);
	txSize = sprintf(txBuffer, "high: %d %d %d %d %d %d %d %d\n", high[0], high[1], high[2], high[3], high[4], high[5], high[6], high[7]);
	uart.transit(txBuffer, txSize);

	for (int i = 0; i < 8; i++)
	{
		threshold[i] = (high[i] - low[i]) / 2 + low[i];
		graySensor.setThreshold(i, threshold[i]);
	}

	while (uart.isTransiting())
		vTaskDelay(1);
	txSize = sprintf(txBuffer, "threshold: %d %d %d %d %d %d %d %d\n", threshold[0], threshold[1], threshold[2], threshold[3], threshold[4], threshold[5], threshold[6], threshold[7]);
	uart.transit(txBuffer, txSize);
	while (uart.isTransiting())
		vTaskDelay(1);

	void keyThread(void*);
	xTaskCreate(keyThread, "key", 0x100, nullptr, 1, nullptr);
}
