#include "ti_msp_dl_config.h"

#include "algorithm"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "servo.hpp"

extern UART uart;

Servo servo{ { Servo_Pwm_INST, GPIO_Servo_Pwm_C1_IDX } };

void servemoterThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("servemoterThread started\n", 25);

	servo.mixer.setMixCallback([]() { servo.pwm = std::clamp<unsigned short>((unsigned short)(float)servo.mixer, 1500, 2600); });

	while (true)
	{
		servo.mix();
		vTaskDelay(10);
	}
}
