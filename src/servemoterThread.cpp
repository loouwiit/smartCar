#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "serve.hpp"

static char txBuffer[64]{};
static int txSize{};

extern UART uart;

Serve serve{ { Servomotor_Pwm_INST, GPIO_Servomotor_Pwm_C1_IDX } };  // 500 - 2500

void servemoterThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("servemoterThread started\n", 25);

	serve.mixer.setMixCallback([]() { serve.pwm = (unsigned short)(float)serve.mixer; });

	while (true)
	{
		serve[Serve::MixNumber::Script] = serve.getScriptTotol(xTaskGetTickCount());
		serve.mix();
		vTaskDelay(10);
	}
}
