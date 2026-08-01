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
#include "script.hpp"
#include "mutex.hpp"

extern UART uart;
extern Mixer<float, MixNumber::Count> mixer[2];
extern Script<> script[2];

extern volatile bool grayEnable;
extern volatile bool calibrating;

extern volatile TickType_t oledTimeStart;

extern volatile int moveCount[2];
extern TaskHandle_t calibrateTaskHandle;
extern Mutex* mutex;
static int targetCount{};
constexpr int roundCount = 10370;
constexpr TickType_t StartTimeCoolDown = pdMS_TO_TICKS(2000);

constexpr static float KeySpeed = 2.5f;
constexpr static TickType_t longPressTime = pdMS_TO_TICKS(2000);
static TickType_t keyPressTime = 0;

static void keyPress();
static void keyRelease();
volatile bool started = true;
void stop(); // turn

void keyThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	vTaskDelay(10);

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("key started\n", 12);

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

		int scriptIndex[2]{};
		Script<>::ScriptEntry* scriptEntry[2]{};

		{
			Lock lock{ *mutex };
			targetCount = moveCount[0] + moveCount[1] + roundCount * 2;
		}

		while (true)
		{
			{
				Lock lock{ *mutex };
				scriptIndex[0] = script[0].getFreeScriptEntryIndex();
				scriptIndex[1] = script[1].getFreeScriptEntryIndex();
				if (scriptIndex[0] != -1 && scriptIndex[1] != -1)
					break;
			}
			while (uart.isTransiting())
				vTaskDelay(1);
			uart.transit("cannot find free script entry!\n", 31);
			vTaskDelay(1);
		}

		{
			Lock lock{ *mutex };

			scriptEntry[0] = &script[0][scriptIndex[0]];
			scriptEntry[1] = &script[1][scriptIndex[1]];

			for (auto& i : scriptEntry)
			{
				i->strength = +KeySpeed;
				i->duration = portMAX_DELAY;
				i->delay = 0;
				i->stopCount = targetCount;
			}

			oledTimeStart = xTaskGetTickCount();

			mixer[0].enable(MixNumber::GraySensor);
			mixer[1].enable(MixNumber::GraySensor);
		}

		// 运行中禁用按键回调，防止误触（stop() 时会恢复）
		GpioInterrupt::setCallback(KEY_PORT, KEY_KEY_1_PIN, nullptr, nullptr);

		while (uart.isTransiting())
			vTaskDelay(1);
		uart.transit("started\n", 8);

		while (true)
		{
			{
				Lock lock{ *mutex };
				if (scriptEntry[0]->strength == 0 && scriptEntry[1]->strength == 0)
					break;
			}
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}
}

void keyPress()
{
	keyPressTime = xTaskGetTickCountFromISR();
}

void keyRelease()
{
	auto pressTime = xTaskGetTickCountFromISR() - keyPressTime;
	if (pressTime > longPressTime)
	{
		calibrating = true;

		// 通过任务通知唤醒 calibrateThread（任务在启动时创建，ISR 只发信号）
		BaseType_t higherPriorityTaskWoken = pdFALSE;
		if (calibrateTaskHandle != nullptr)
		{
			xTaskNotifyFromISR(calibrateTaskHandle, 0, eNoAction, &higherPriorityTaskWoken);
			portYIELD_FROM_ISR(higherPriorityTaskWoken);
		}
	}
	else if (!started && !calibrating)
	{
		started = true;
	}
}

void stop()
{
	if (!started) return;
	started = false;

	GpioInterrupt::setCallback(KEY_PORT, KEY_KEY_1_PIN, keyRelease, keyPress);

	{
		Lock lock{ *mutex };
		grayEnable = false;

		script[0].clear();
		script[1].clear();

		mixer[0].disable(MixNumber::GraySensor);
		mixer[1].disable(MixNumber::GraySensor);
	}
}
