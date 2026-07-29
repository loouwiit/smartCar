#include "ti_msp_dl_config.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "autoDeleteThread.hpp"
#include "uart.hpp"

extern UART uart;
extern float pitch, roll, yaw;

static char txBuffer[64]{};
static int txSize = 0;

extern "C" int MPU6050_Init(void);

void mpu6050Thread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	vTaskDelay(10);
	
	if (MPU6050_Init() < 0)
	{
		while (uart.isTransiting())
			vTaskDelay(1);
		uart.transit("mpu failed\n", 11);
		return;
	}

	void initMpuInterrupt();
	initMpuInterrupt();

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("mpu started\n", 12);
}
