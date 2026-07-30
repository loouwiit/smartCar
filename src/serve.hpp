#pragma once

#include "pwm.hpp"
#include "mixer.hpp"

#include <FreeRTOS.h>

class Serve
{
public:
	Serve() = default;
	Serve(PWM pwm) : pwm{ pwm } { mixer[MixNumber::Standard] = StandardBalancePoint; }

	constexpr static unsigned short StandardBalancePoint = 2335;

	class MixNumber
	{
	public:
		enum : unsigned char {
			Uart,
			Standard = Uart,
			BalanceFeed,
			Script,

			Count,
		};
	};

	struct ScriptEntry
	{
		float strength{};
		TickType_t startTime{ portMAX_DELAY };
		TickType_t expireTime{};
	};

	auto& mix() { return mixer.mix(); }
	operator uint32_t() { return (uint32_t)pwm; }

	auto& operator[](unsigned number) { return mixer[number]; }
	auto& getMixer(unsigned number) { return mixer[number]; }
	auto& getScriptEntry(unsigned index) { return script[index]; }
	auto getFreeScriptEntryIndex()
	{
		for (int i = 0; i < ScriptCapacity; ++i) if (script[i].startTime == portMAX_DELAY) return i;
		return -1;
	}

	auto getScriptTotol(TickType_t nowTime)
	{
		float scriptTotol = 0;
		for (auto& i : script) if (i.startTime < nowTime)
		{
			if (i.expireTime < nowTime) i = {};
			else scriptTotol += i.strength;
		}
		return scriptTotol;
	}

	constexpr static int ScriptCapacity = 5;

	PWM pwm{};
	Mixer<float, MixNumber::Count>mixer{};
	ScriptEntry script[ScriptCapacity];
};
