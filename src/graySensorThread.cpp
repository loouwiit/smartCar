#include "ti_msp_dl_config.h"

#include "autoDeleteThread.hpp"
#include "uart.hpp"
#include "fpid.hpp"
#include "mixer.hpp"
#include "gpio.hpp"
#include "graySensor.hpp"

#include "cstdio"

extern UART uart;
GraySensor graySensor = { {GraySensor_GraySensorA_PORT, GraySensor_GraySensorA_PIN},{GraySensor_GraySensorB_PORT, GraySensor_GraySensorB_PIN},{GraySensor_GraySensorC_PORT, GraySensor_GraySensorC_PIN}, GraySensorAdc_INST,GraySensorAdc_ADCMEM_Cha };

char txBuffer[64]{};
int txSize = 0;
extern FPID fpid[2];
extern Mixer<float, MixNumber::Count> mixer[2];
float grayMoveSpeed = 0.1f;

bool grayEnable = false;

constexpr TickType_t TurnTimeCoolDown = 2000;
TickType_t nextTurnTime = 0;
extern int turnContorl;
extern int leftTurnCount;

static bool keyDisabled = false;

void graySensorThread(void*)
{
	AutoDeleteThread autoDeleteThread{};

	while (uart.isTransiting()) {}
	uart.transit("gray started\n", 13);

	float graySpeed = 0;
	float gapSpeed = 0.0f;
	float lastGray = 0;

	//	810	1496	2072	1732	1525	2226	1816	1008
	//	276	513	740	505	314	723	677	322

	graySensor.setThreshold(0, 583.05);
	graySensor.setThreshold(1, 1078.225);
	graySensor.setThreshold(2, 1505.9);
	graySensor.setThreshold(3, 1210.525);
	graySensor.setThreshold(4, 1010.325);
	graySensor.setThreshold(5, 1587.225);
	graySensor.setThreshold(6, 1331.925);
	graySensor.setThreshold(7, 716.45);

	while (true)
	{
		graySensor.update();

		if (!grayEnable)
		{
			vTaskDelay(100);
			continue;
		}

		if (graySensor[0] && graySensor[1] && graySensor[2] && graySensor[3] && graySensor[4] && graySensor[5] && graySensor[6] && graySensor[7])
		{
			// 无效数据，全是1，常见于悬空
			nextTurnTime = 0;
			vTaskDelay(5);
			continue;
		}

		graySpeed = 0.0f;

		if (!graySensor[0] && !graySensor[1] && !graySensor[2] && !graySensor[3] && !graySensor[4] && !graySensor[5] && !graySensor[6] && !graySensor[7])
		{
			// 无效数据，全是0，常见于跑飞
			graySpeed = grayMoveSpeed * 5;
			// 向右运动
		}

		if (graySensor[0])
			graySpeed -= grayMoveSpeed * 5;
		else if (graySensor[1])
			graySpeed -= grayMoveSpeed * 4;
		else if (graySensor[2])
			graySpeed -= grayMoveSpeed * 2;
		else if (graySensor[3])
			graySpeed -= grayMoveSpeed * 1;

		if (graySensor[7])
			graySpeed += grayMoveSpeed * 5;
		else if (graySensor[6])
			graySpeed += grayMoveSpeed * 4;
		else if (graySensor[5])
			graySpeed += grayMoveSpeed * 2;
		else if (graySensor[4])
			graySpeed += grayMoveSpeed * 1;

		if (lastGray != graySpeed)
		{
			lastGray = graySpeed;

			mixer[0][MixNumber::GraySensor] = +graySpeed;
			mixer[1][MixNumber::GraySensor] = -graySpeed;

			mixer[0].mix();
			mixer[1].mix();
		}

		if (graySensor[0] && graySensor[1] && graySensor[2] && (xTaskGetTickCount() > nextTurnTime))
		{
			while (uart.isTransiting())
				vTaskDelay(1);
			uart.transit("turn\n", 5);
			vTaskDelay(500);
			nextTurnTime = xTaskGetTickCount() + TurnTimeCoolDown;
			turnContorl = leftTurnCount;
		}

		vTaskDelay(5);
	}
}
