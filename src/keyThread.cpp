#include "ti_msp_dl_config.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "capture.hpp"
#include "motor.hpp"
#include "gpioInterrupt.hpp"
#include "fpid.hpp"
#include "mixer.hpp"

extern UART uart;
extern Mixer<float, MixNumber::Count> mixer[2];

extern bool grayEnable;

extern unsigned turnRoundCount;
extern int turnContorl;

extern TickType_t nextTurnTime;
constexpr TickType_t StartTimeCoolDown = 2000;

constexpr static float KeySpeed = 2.0f;
constexpr static TickType_t longPressTime = 2000;
static TickType_t keyPressTime = 0;

static void keyPress();
static void keyRelease();
bool started = true;
void start(); // key
void stop(); // turn

void keyThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	vTaskDelay(10);
	GpioInterrupt::setCallback(KEY_PORT, KEY_KEY_1_PIN, keyRelease, keyPress);

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("key started\n", 14);

	stop();

	vTaskDelay(103);
}

void keyPress()
{
	keyPressTime = xTaskGetTickCount();
}

void keyRelease()
{
	auto pressTime = xTaskGetTickCount() - keyPressTime;
	if (pressTime > longPressTime)
	{
		void calibrateThread(void*);
		xTaskCreate(calibrateThread, "calibrate", 0x100, nullptr, 2, nullptr);
		GpioInterrupt::setCallback(KEY_PORT, KEY_KEY_1_PIN, nullptr, nullptr);
	}
	else
	{
		turnRoundCount = 4;
		start();
	}
}

void start()
{
	if (started) return;
	started = true;

	grayEnable = true;

	nextTurnTime = xTaskGetTickCount() + StartTimeCoolDown;
	turnContorl = 0;

	mixer[0][MixNumber::Key] = +KeySpeed;
	mixer[1][MixNumber::Key] = +KeySpeed;

	mixer[0].enable(MixNumber::GraySensor);
	mixer[1].enable(MixNumber::GraySensor);
	mixer[0].enable(MixNumber::Turn);
	mixer[1].enable(MixNumber::Turn);
}

void stop()
{
	if (!started) return;
	started = false;

	grayEnable = false;

	mixer[0][MixNumber::Key] = 0.0f;
	mixer[1][MixNumber::Key] = 0.0f;

	mixer[0][MixNumber::Turn] = 0;
	mixer[1][MixNumber::Turn] = 0;

	mixer[0].disable(MixNumber::GraySensor);
	mixer[1].disable(MixNumber::GraySensor);
	mixer[0].disable(MixNumber::Turn);
	mixer[1].disable(MixNumber::Turn);
}
