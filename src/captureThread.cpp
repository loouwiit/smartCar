#include "ti_msp_dl_config.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include <semphr.h>

#include "capture.hpp"
#include "autoDeleteThread.hpp"
#include "uart.hpp"
#include "gpio.hpp"

void captureThread(void*);
unsigned getTime(); // export
float captureSpeeds[2]{}; // export
int moveCount[2]{}; // export

Capture capture{ EncoderCapture_INST, EncoderCapture_INST_INT_IRQN };

constexpr static unsigned TimerResolution = 1;
constexpr static unsigned TimerOverflowerTime = 50 * 1000;
constexpr static unsigned TimerExpireTime = 100 * 1000;

static unsigned overflowTime = TimerExpireTime; // 防止负溢出
static unsigned lastTimes[2] = {};
static GPIO gpios[2]{ {EncoderGpio_EncoderGpioA_PORT, EncoderGpio_EncoderGpioA_PIN},
	{EncoderGpio_EncoderGpioB_PORT, EncoderGpio_EncoderGpioB_PIN} };

extern UART uart;

static void captureCallback(Timer::TimerInstance timer, Timer::TimerEvent event, unsigned& overflowTime, unsigned* lastTimes, GPIO* gpios);

unsigned getTime()
{
	return capture.getCount() * TimerResolution + overflowTime;
}

void captureThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	Capture::setCallbackFunction({ EncoderCapture_INST }, [](Timer::TimerEvent event) { captureCallback(EncoderCapture_INST, event, overflowTime, lastTimes, gpios); });
	capture.start();

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("capture started\n", 16);

	unsigned nowTime = overflowTime;
	unsigned expireTime = TimerExpireTime;

	while (true)
	{
		nowTime = getTime();
		expireTime = nowTime - TimerExpireTime;
		for (int i = 0; i < 2;i++)
		{
			if (lastTimes[i] < expireTime)
				captureSpeeds[i] = 0;
		}

		vTaskDelay(10);
	}

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("capture stopped\n", 16);

	capture.stop();
	capture.setCallbackFunction({ EncoderCapture_INST }, nullptr);
}

static void captureCallback(Timer::TimerInstance timer, Timer::TimerEvent event, unsigned& overflowTime, unsigned* lastTimes, GPIO* gpios)
{
	using Event = Timer::TimerEvent;

	switch (event)
	{
	case Event::DL_TIMER_IIDX_CC0_UP:
	case Event::DL_TIMER_IIDX_CC0_DN:
	{
		unsigned nowTime = overflowTime + Timer::getCc(timer, Timer::TimerCcIndex::DL_TIMER_CC_0_INDEX) * TimerResolution;
		unsigned deltaTime = nowTime - lastTimes[0];
		lastTimes[0] = nowTime;

		if (gpios[0])
		{
			captureSpeeds[0] = +1000000.0f / deltaTime / 13 / 20.409f;
			moveCount[0]++;
		}
		else
		{
			captureSpeeds[0] = -1000000.0f / deltaTime / 13 / 20.409f;
			moveCount[0]--;
		}
		break;
	}
	case Event::DL_TIMER_IIDX_CC1_UP:
	case Event::DL_TIMER_IIDX_CC1_DN:
	{
		unsigned nowTime = overflowTime + Timer::getCc(timer, Timer::TimerCcIndex::DL_TIMER_CC_1_INDEX) * TimerResolution;
		unsigned deltaTime = nowTime - lastTimes[1];
		lastTimes[1] = nowTime;

		if (gpios[1])
		{
			captureSpeeds[1] = +1000000.0f / deltaTime / 13 / 20.409f;
			moveCount[1]++;
		}
		else
		{
			captureSpeeds[1] = -1000000.0f / deltaTime / 13 / 20.409f;
			moveCount[1]--;
		}
		break;
	}
	case Event::DL_TIMER_IIDX_LOAD:
	{
		overflowTime += TimerOverflowerTime;
		break;
	}
	default:
		break;
	}
};