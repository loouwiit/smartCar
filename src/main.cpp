#include "ti_msp_dl_config.h"

#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "gpioInterrupt.hpp"
#include "gpio.hpp"
#include <cstring>
#include "mutex.hpp"

constexpr size_t SystemRxBufferSize = 256;
constexpr size_t SystemRxBufferCount = 4;

constexpr size_t UserRxBufferSize = 256;
constexpr size_t UserRxBufferCount = 16;

UART uart{};
UART uartData{};
Mutex* mutex{};

TaskHandle_t calibrateTaskHandle = nullptr;

void mainThread(void*);
void captureThread(void*);
void motorThread(void*);
void uartThread(void*);
void graySensorThread(void*);
void keyThread(void*);
void servoThread(void*);
void oledThread(void*);
void balanceThread(void*);
void calibrateThread(void*);

int main(void)
{
	SYSCFG_DL_init();

	xTaskCreate(mainThread, "main", 0x100, nullptr, 1, nullptr);

	vTaskStartScheduler();

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("scheduler exit!\n", 16);
	while (1)
		;
}

void mainThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	mutex = new Mutex{};

	void uartInit();
	uartInit();
	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("system started\n", 15);

	GpioInterrupt::init();

	auto createTask = [](TaskFunction_t function, const char* name, uint32_t stackDepth, UBaseType_t priority, TaskHandle_t* handle)
		{
			if (xTaskCreate(function, name, stackDepth, nullptr, priority, handle) != pdPASS)
				while (true)
				{
					while (uart.isTransiting())
						vTaskDelay(1);
					uart.transit("task create failed!\n", 20);
				};
		};

	createTask(captureThread, "capture", 0x100, 2, nullptr);
	createTask(motorThread, "motor", 0x100, 2, nullptr);
	createTask(uartThread, "uart", 0x200, 2, nullptr);
	createTask(graySensorThread, "graySensor", 0x100, 1, nullptr);
	createTask(keyThread, "key", 0x100, 1, nullptr);
	createTask(servoThread, "servo", 0x100, 1, nullptr);
	createTask(oledThread, "oled", 0x200, 1, nullptr);
	createTask(balanceThread, "balanceThread", 0x100, 1, nullptr);
	// 提前创建校准任务：启动时阻塞等待任务通知，由按键长按（中断）唤醒
	createTask(calibrateThread, "calibrate", 0x100, 2, &calibrateTaskHandle);
}

void uartInit()
{
	NVIC_ClearPendingIRQ(UartSystem_INST_INT_IRQN);
	NVIC_EnableIRQ(UartSystem_INST_INT_IRQN);

	uart = UART{ {UartSystem_INST}, DMA_RX_SYSTEM_CHAN_ID, SystemRxBufferSize, SystemRxBufferCount, DMA_TX_SYSTEM_CHAN_ID };

	NVIC_ClearPendingIRQ(UartData_INST_INT_IRQN);
	NVIC_EnableIRQ(UartData_INST_INT_IRQN);

	uartData = UART{ { UartData_INST }, DMA_RX_DATA_CHAN_ID, UserRxBufferSize, UserRxBufferCount, DMA_TX_DATA_CHAN_ID };
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
	// 某个任务栈溢出（用调试器查看 xTask / pcTaskName 定位是哪个任务）。
	// 栈已损坏，没有安全恢复手段，停机。
	(void)xTask;
	while (1)
	{
		while (uart.isTransiting())
			;
		uart.transit("\nstack overflow in ", 19);
		while (uart.isTransiting())
			;
		uart.transit(pcTaskName, strlen(pcTaskName));
	}
}
