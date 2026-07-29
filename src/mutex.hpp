#error 没写完

// #pragma once

// #include <FreeRTOS.h>
// #include <semphr.h>

// class Mutex
// {
// public:
// 	Mutex() { semaphore = xSemaphoreCreateMutex(); }
// 	~Mutex() { vSemaphoreDelete(semaphore); }

// 	bool get(TickType_t maxWaitTick = 0)
// 	{
// 		return xSemaphoreTake(uartSemaphore, maxWaitTick) == pdTRUE;
// 	}

// 	void release()
// 	{
// 		xSemaphoreGive(semaphore);
// 	}

// private:
// 	SemaphoreHandle_t semaphore;
// };

// class Lock
// {
// public:
// 	Lock(Mutex& mutex) : mutex{ mutex }
// 	{
// 		mutex.get(portMAX_DELAY);
// 	}

// 	~Lock()
// 	{
// 		mutex.rela
// 	}

// private:
// 	Mutex& mutex;
// };
