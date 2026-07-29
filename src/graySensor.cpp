#include "ti_msp_dl_config.h"

#include <FreeRTOS.h>
#include <task.h>

#include "graySensor.hpp"

GraySensor& GraySensor::update()
{
	for (int i = 0; i < 8; i++)
	{
		address[0] = i & (1 << 2);
		address[1] = i & (1 << 1);
		address[2] = i & (1 << 0);

		value[i] = read();

		state[i] = value[i] < threshold[i];
	}
	return *this;
}

uint16_t GraySensor::read()
{
	DL_ADC12_enableConversions(adc);
	//软件触发ADC开始转换
	DL_ADC12_startConversion(adc);

	//如果当前状态 不是 空闲状态
	while (DL_ADC12_getStatus(adc) != DL_ADC12_STATUS_CONVERSION_IDLE) {}

	//清除触发转换状态
	DL_ADC12_stopConversion(adc);
	//失能ADC转换
	DL_ADC12_disableConversions(adc);

	//获取数据
	return DL_ADC12_getMemResult(adc, adcChannel);
}
