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
			MovementScript,

			Count,
		};
	};

	struct ScriptEntry
	{
		float strength{};
		TickType_t time{};
	};

	auto& mix() { return mixer.mix(); }
	operator uint32_t() { return (uint32_t)pwm; }

	auto& operator[](unsigned number) { return mixer[number]; }
	auto& getMixer(unsigned number) { return mixer[number]; }
	auto& getScriptEntry(unsigned index) { return script[index]; }

	PWM pwm{};
	Mixer<float, MixNumber::Count>mixer{};
	ScriptEntry script[3];
};
