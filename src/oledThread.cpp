#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "oled_hardware_i2c.h"

static char buffer[128]{};
static int bufferSize{};

extern UART uart;
extern int moveCount[2];

void oledThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("oledThread started\n", 19);

	OLED_Init();
	while (true)
	{
		sprintf(buffer, "move count: %d %d", moveCount[0], moveCount[1]);
		OLED_ShowString(0, 0, buffer, 8);

		sprintf(buffer, "tick: %ld", xTaskGetTickCount() / 1000);
		OLED_ShowString(0, 1, buffer, 8);

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
