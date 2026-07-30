#pragma once

#include <FreeRTOS.h>

template <class T, unsigned Size = 5>
class Script
{
public:
	struct ScriptEntry
	{
		T strength{};
		TickType_t startTime{ portMAX_DELAY };
		TickType_t expireTime{};
	};

	ScriptEntry script[Size];

	auto& operator[](unsigned index) { return script[index]; }

	auto getFreeScriptEntryIndex()
	{
		for (int i = 0; i < Size; ++i) if (script[i].startTime == portMAX_DELAY) return i;
		return -1;
	}

	auto getScriptTotol(TickType_t nowTime)
	{
		float scriptTotol = 0;
		for (auto& i : script) if (i.startTime < nowTime)
		{
			if (i.expireTime < nowTime) i = {}; // 过期
			else scriptTotol += i.strength; // 有效
		}
		return scriptTotol;
	}
};
