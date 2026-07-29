#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "oled_hardware_i2c.h"

extern UART uart;

void oledThread(void*)
{
	AutoDeleteThread autoDeleteThread{};
	char txBuffer[128]{};
	int txSize{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("oledThread started\n", 19);

	OLED_Init();
	OLED_ShowString(0, 7, (uint8_t*)"test", 8);

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("test showed\n", 12);
}
