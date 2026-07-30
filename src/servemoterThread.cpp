#include "ti_msp_dl_config.h"

#include "algorithm"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "serve.hpp"

extern UART uart;

Serve serve{ { Servomotor_Pwm_INST, GPIO_Servomotor_Pwm_C1_IDX } };

void servemoterThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("servemoterThread started\n", 25);

	serve.mixer.setMixCallback([]() { serve.pwm = std::clamp<unsigned short>((unsigned short)(float)serve.mixer, 1500, 2600); });

	while (true)
	{
		serve.mix();
		vTaskDelay(10);
	}
}
