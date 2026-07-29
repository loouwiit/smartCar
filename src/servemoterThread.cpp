#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "pwm.hpp"

extern UART uart;

PWM serve{ Servomotor_Pwm_INST, GPIO_Servomotor_Pwm_C1_IDX };

void servemoterThread(void*)
{
	AutoDeleteThread autoDeleteThread{};
	char txBuffer[128]{};
	int txSize{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("servemoterThread started\n", 25);

	auto target = 0;
	auto direction = +20;
	while (true)
	{
		target += direction;
		if (target >= 3000)
		{
			target = 3000 - 1;
			direction = -20;
		}
		if (target < 000)
		{
			target = 000;
			direction = +20;
		}

		serve = target;
		vTaskDelay(pdMS_TO_TICKS(100));

		auto txSize = sprintf(txBuffer, "target: %d\n", target);
		while (uart.isTransiting())
			vTaskDelay(1);
		uart.transit(txBuffer, txSize);
	}
}
