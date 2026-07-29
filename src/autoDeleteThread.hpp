#pragma once

#include <FreeRTOS.h>
#include <task.h>

class AutoDeleteThread
{
public:
	~AutoDeleteThread() { vTaskDelete(nullptr); }
};
