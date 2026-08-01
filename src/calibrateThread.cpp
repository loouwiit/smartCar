#include "ti_msp_dl_config.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart.hpp"
#include "graySensor.hpp"
#include "autoDeleteThread.hpp"

#include "oled_hardware_i2c.h"

#include "cstdio"

extern UART uart;
extern GraySensor graySensor;

volatile bool calibrating{};

static GPIO key{ KEY_PORT, KEY_KEY_1_PIN };
static GPIO led{ LED_PORT,LED_G_PIN };

void calibrateThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	char txBuffer[128]{};
	int txSize{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("calibrateThread started\n", 24);

	// 任务常驻：每次校准完成后回到循环顶部，等待下一次长按通知（可反复校准）
	while (true)
	{
		// 阻塞等待按键长按触发的任务通知（由 keyRelease 中断发出）
		// pdTRUE：返回前清空通知计数，避免积压导致校准刚结束又被立即触发
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		while (key == true) // release
		{
			led.toggle();
			vTaskDelay(100);
		}

		while (key == false) // press
			vTaskDelay(100);

		vTaskDelay(2000);
		for (int i = 0; i < 8; i++)
			graySensor.setLow(i);

		txSize = sprintf(txBuffer, "low: %d %d %d %d %d %d %d %d\n", graySensor.getLow(0), graySensor.getLow(1), graySensor.getLow(2), graySensor.getLow(3), graySensor.getLow(4), graySensor.getLow(5), graySensor.getLow(6), graySensor.getLow(7));

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
			graySensor.setHigh(i);

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "high: %d %d %d %d %d %d %d %d\n", graySensor.getHigh(0), graySensor.getHigh(1), graySensor.getHigh(2), graySensor.getHigh(3), graySensor.getHigh(4), graySensor.getHigh(5), graySensor.getHigh(6), graySensor.getHigh(7));
		uart.transit(txBuffer, txSize);

		while (uart.isTransiting())
			vTaskDelay(1);
		txSize = sprintf(txBuffer, "threshold: %d %d %d %d %d %d %d %d\n", graySensor.getThreshold(0), graySensor.getThreshold(1), graySensor.getThreshold(2), graySensor.getThreshold(3), graySensor.getThreshold(4), graySensor.getThreshold(5), graySensor.getThreshold(6), graySensor.getThreshold(7));
		uart.transit(txBuffer, txSize);
		while (uart.isTransiting())
			vTaskDelay(1);

		sprintf(txBuffer, "%d %d %d %d", graySensor.getThreshold(0), graySensor.getThreshold(1), graySensor.getThreshold(2), graySensor.getThreshold(3));
		OLED_ShowString(0, 2, txBuffer, 8);
		sprintf(txBuffer, "%d %d %d %d", graySensor.getThreshold(4), graySensor.getThreshold(5), graySensor.getThreshold(6), graySensor.getThreshold(7));
		OLED_ShowString(0, 3, txBuffer, 8);

		calibrating = false;
	}
}
