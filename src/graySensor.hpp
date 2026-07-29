#pragma once
#include "gpio.hpp"

class GraySensor
{
public:
	GraySensor(GPIO addressA, GPIO addressB, GPIO addressC, ADC12_Regs* adc, DL_ADC12_MEM_IDX adcChannel) : address{ addressA, addressB, addressC }, adc{ adc }, adcChannel{ adcChannel } {}

	bool operator[](unsigned char index) { return state[index]; }
	uint16_t getAdcValue(unsigned char index) { return value[index]; }

	void setThreshold(unsigned char index, uint16_t threshold) { this->threshold[index] = threshold; }

	GraySensor& update();
	GraySensor& operator()() { update(); return *this; }

private:
	using Microsecond = unsigned int; // us
	using Milisecond = unsigned long; // ms
	constexpr static Milisecond ResetTime = 2;

	uint16_t read();

	GPIO address[3]{};
	ADC12_Regs* adc = nullptr;
	DL_ADC12_MEM_IDX adcChannel = DL_ADC12_MEM_IDX_0;

	uint16_t threshold[8]{};
	uint16_t value[8]{};
	bool state[8]{};
};
