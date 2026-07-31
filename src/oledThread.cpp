#include "ti_msp_dl_config.h"

#include "stdio.h"
#include <FreeRTOS.h>
#include <task.h>

#include "uart.hpp"
#include "autoDeleteThread.hpp"
#include "oled_hardware_i2c.h"

static char buffer[32]{};
constexpr static TickType_t oledTimeStartCoolDown = 1000;
TickType_t oledTimeStart{}; // 启动计时判定
TickType_t oledTimeStop{}; // 结束计时判定

extern UART uart;
extern int moveCount[2];

void oledThread(void*)
{
	AutoDeleteThread autoDeleteThread{};
	int lastMoveCount{};
	TickType_t StopDetectTime{};

	while (uart.isTransiting())
		vTaskDelay(1);
	uart.transit("oledThread started\n", 19);
	
	OLED_Init();
	OLED_ShowString(0, 0, "time: 0", 8);
	while (true)
	{
		auto nowTime = xTaskGetTickCount();
		int nowMoveCount = moveCount[0] + moveCount[1];

		// 若正在记录，则更新时间
		if (oledTimeStart != portMAX_DELAY && oledTimeStop == portMAX_DELAY)
		{
			// 尝试判定结束
			if (StopDetectTime != portMAX_DELAY) do
			{
				if (nowTime < StopDetectTime) break; // 刚刚启动，不予判定
				if (lastMoveCount == nowMoveCount)
					oledTimeStop = nowTime; // 结束
				lastMoveCount = nowMoveCount;
			} while (false);
			else StopDetectTime = nowTime + StopDetectTime;

			sprintf(buffer, "time: %ld", ((oledTimeStop == portMAX_DELAY ? nowTime : oledTimeStop) - oledTimeStart) / 1000);
			OLED_ShowString(0, 0, buffer, 8);
		}

		sprintf(buffer, "move count: %d", nowMoveCount);
		OLED_ShowString(0, 1, buffer, 8);

		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
