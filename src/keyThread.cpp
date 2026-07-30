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
extern bool calibrating;

extern int moveCount[2];
static int targetCount{};
constexpr int roundCount = 10370;
constexpr TickType_t StartTimeCoolDown = pdMS_TO_TICKS(2000);

constexpr static float KeySpeed = 2.5f;
constexpr static TickType_t longPressTime = pdMS_TO_TICKS(2000);
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

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("key started\n", 14);

	while (true)
	{
		stop();

		while (!started)
			vTaskDelay(pdMS_TO_TICKS(100));

		while (uart.isTransiting())
			vTaskDelay(1);
		uart.transit("starting\n", 9);

		vTaskDelay(StartTimeCoolDown);
		grayEnable = true;

		targetCount = moveCount[0] + moveCount[1] + roundCount * 2;

		mixer[0][MixNumber::Key] = +KeySpeed;
		mixer[1][MixNumber::Key] = +KeySpeed;

		mixer[0].enable(MixNumber::GraySensor);
		mixer[1].enable(MixNumber::GraySensor);

		while (moveCount[0] + moveCount[1] < targetCount)
			vTaskDelay(pdMS_TO_TICKS(100));
	}
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
		calibrating = true;
		void calibrateThread(void*);
		xTaskCreate(calibrateThread, "calibrate", 0x100, nullptr, 2, nullptr);
	}
	else start();
}

void start()
{
	if (started || calibrating) return;
	started = true;

	GpioInterrupt::setCallback(KEY_PORT, KEY_KEY_1_PIN, nullptr, nullptr);
}

void stop()
{
	if (!started) return;
	started = false;

	GpioInterrupt::setCallback(KEY_PORT, KEY_KEY_1_PIN, keyRelease, keyPress);

	grayEnable = false;

	mixer[0][MixNumber::Key] = 0.0f;
	mixer[1][MixNumber::Key] = 0.0f;

	mixer[0].disable(MixNumber::GraySensor);
	mixer[1].disable(MixNumber::GraySensor);
}
