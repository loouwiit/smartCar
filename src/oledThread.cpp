#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "oled_hardware_i2c.h"

static char buffer[32]{};
constexpr static TickType_t oledTimeStartCoolDown = pdMS_TO_TICKS(1000);
volatile TickType_t oledTimeStart{ portMAX_DELAY }; // 启动计时判定
volatile TickType_t oledTimeStop{ portMAX_DELAY }; // 结束计时判定

extern UART uart;
extern volatile int moveCount[2];

void oledThread(void*)
{
	AutoDeleteThread autoDeleteThread{};
	int lastMoveCount{};
	TickType_t lastStartTime{ portMAX_DELAY };
	TickType_t stopDetectTime{ portMAX_DELAY };

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("oledThread started\n", 19);

	OLED_Init();
	OLED_ShowString(0, 0, "time: 0", 8);
	while (true)
	{
		auto nowTime = xTaskGetTickCount();
		int nowMoveCount = moveCount[0] + moveCount[1];

		// 检测是否为新计时
		if (oledTimeStart != lastStartTime)
		{
			stopDetectTime = nowTime + oledTimeStartCoolDown;
			oledTimeStop = portMAX_DELAY;
			lastStartTime = oledTimeStart;
		}

		// 若正在记录，则更新时间
		if (oledTimeStart != portMAX_DELAY && oledTimeStop == portMAX_DELAY)
		{
			// 尝试判定结束
			if (stopDetectTime != portMAX_DELAY) do
			{
				if (nowTime < stopDetectTime) break; // 刚刚启动，不予判定
				if (lastMoveCount == nowMoveCount)
					oledTimeStop = nowTime; // 结束
				lastMoveCount = nowMoveCount;
			} while (false);

			sprintf(buffer, "time: %ld   ", ((oledTimeStop == portMAX_DELAY ? nowTime : oledTimeStop) - oledTimeStart) / 100);
			OLED_ShowString(0, 0, buffer, 8);
		}

		sprintf(buffer, "move count: %d", nowMoveCount);
		OLED_ShowString(0, 1, buffer, 8);

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
