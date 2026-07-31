#pragma once

#include <FreeRTOS.h>

extern int moveCount[2];

template <class T = float, unsigned Size = 10>
class Script
{
public:
	struct ScriptEntry
	{
		T strength{}; // 有效性依据
		TickType_t duration{};
		TickType_t delay{};
		int startCount{ INT32_MIN };
		int stopCount{ INT32_MAX };

	private:
		TickType_t startTime{ portMAX_DELAY }; // 激活依据
		TickType_t expireTime{ 0 };

		friend class Script;
	};

	ScriptEntry script[Size]{};

	auto& operator[](unsigned index) { return script[index]; }

	void clear()
	{
		for (auto& i : script) i = {};
	}

	int getFreeScriptEntryIndex()
	{
		for (unsigned i = 0; i < Size; ++i) if (script[i].strength == 0) return i;
		return -1;
	}

	auto getScriptTotol(TickType_t nowTime)
	{
		float scriptTotol = 0;
		int nowCount = moveCount[0] + moveCount[1];

		for (auto& i : script)
		{
			if (i.strength == 0)
				continue; // 无效数据

			if (i.startTime == portMAX_DELAY &&
				i.startCount < nowCount) // 判定激活
			{
				i.startTime = nowTime + i.delay;
				if (i.duration != portMAX_DELAY)
					i.expireTime = i.startTime + i.duration;
				else i.expireTime = portMAX_DELAY;
			}

			if (i.startTime < nowTime) // 已激活
			{
				if (i.expireTime < nowTime) i = {}; // 判定时间过期
				else if (i.stopCount < nowCount) i = {}; // 判定路程过期
				else scriptTotol += i.strength; // 正常激活
			}
		}
		return scriptTotol;
	}
};
