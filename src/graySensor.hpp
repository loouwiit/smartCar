#pragma once
#include "gpio.hpp"

class GraySensor
{
public:
	GraySensor(GPIO addressA, GPIO addressB, GPIO addressC, ADC12_Regs* adc, DL_ADC12_MEM_IDX adcChannel) : address{ addressA, addressB, addressC }, adc{ adc }, adcChannel{ adcChannel } {}

	bool operator[](unsigned char index) { return state[index]; }
	uint16_t getAdcValue(unsigned char index) { return value[index]; }
	
	void setLow(unsigned char index, uint16_t low) { this->low[index] = low; updateThreshold(index); }
	void setLow(unsigned char index) { setLow(index, getAdcValue(index)); }

	void setHigh(unsigned char index, uint16_t high) { this->high[index] = high; updateThreshold(index); }
	void setHigh(unsigned char index) { setHigh(index, getAdcValue(index)); }

	void setThreshold(unsigned char index, uint16_t threshold) { setLow(index, threshold); setHigh(index, threshold); }

	auto getLow(unsigned char index) { return low[index]; }
	auto getHigh(unsigned char index) { return high[index]; }
	auto getThreshold(unsigned char index) { return threshold[index]; }

	GraySensor& update();
	GraySensor& operator()() { update(); return *this; }

private:
	using Microsecond = unsigned int; // us
	using Milisecond = unsigned long; // ms
	constexpr static Milisecond ResetTime = 2;

	void updateThreshold(unsigned char index) { threshold[index] = (low[index] + high[index]) / 2; }

	uint16_t read();

	GPIO address[3]{};
	ADC12_Regs* adc = nullptr;
	DL_ADC12_MEM_IDX adcChannel = DL_ADC12_MEM_IDX_0;

	uint16_t low[8]{};
	uint16_t high[8]{};
	uint16_t threshold[8]{};
	uint16_t value[8]{};
	bool state[8]{};
};
