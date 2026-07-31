#include "ti_msp_dl_config.h"

#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "gpioInterrupt.hpp"
#include "gpio.hpp"
#include "clock.h"

constexpr size_t SystemRxBufferSize = 256;
constexpr size_t SystemRxBufferCount = 4;

constexpr size_t UserRxBufferSize = 256;
constexpr size_t UserRxBufferCount = 16;

UART uart{};
UART uartData{};

void mainThread(void*);
void captureThread(void*);
void motorThread(void*);
void uartThread(void*);
void graySensorThread(void*);
void keyThread(void*);
void servemoterThread(void*);
void oledThread(void*);
void balanceThread(void*);

int main(void)
{
	SYSCFG_DL_init();
	// SysTick_Init();

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

	void uartInit();
	uartInit();
	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("system started\n", 15);

	GpioInterrupt::init();

	xTaskCreate(captureThread, "capture", 0x100, nullptr, 2, nullptr);
	xTaskCreate(motorThread, "motor", 0x100, nullptr, 2, nullptr);
	xTaskCreate(uartThread, "uart", 0x200, nullptr, 2, nullptr);
	xTaskCreate(graySensorThread, "graySensor", 0x100, nullptr, 1, nullptr);
	xTaskCreate(keyThread, "key", 0x100, nullptr, 1, nullptr);
	xTaskCreate(servemoterThread, "servemoter", 0x100, nullptr, 1, nullptr);
	xTaskCreate(oledThread, "oled", 0x200, nullptr, 1, nullptr);
	xTaskCreate(balanceThread, "balanceThread", 0x100, nullptr, 1, nullptr);
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
