#include "uart.hpp"
#include <ti/driverlib/dl_dma.h>
#include <string.h>

UART::UARTInctance UART::uartInctance[4]{ UART0, UART1, UART2, UART3 };
UART::DMAChannal UART::dmaChannalRx[4]{};
UART::DMAChannal UART::dmaChannalTx[4]{};
UART::Buffer* UART::bufferRx[4]{};
unsigned short UART::bufferRxCount[4]{};
unsigned short UART::bufferRxWriteIndex[4]{};
unsigned short UART::bufferRxReadIndex[4]{};

extern "C" void UART0_IRQHandler()
{
	auto interrupt = DL_UART_Main_getPendingInterrupt(UART0);
	DL_UART_clearInterruptStatus(UART0, interrupt);
	UART::Interrupt(UART::UARTId::UART0, interrupt);
}

extern "C" void UART1_IRQHandler()
{
	auto interrupt = DL_UART_Main_getPendingInterrupt(UART1);
	DL_UART_clearInterruptStatus(UART1, interrupt);
	UART::Interrupt(UART::UARTId::UART1, DL_UART_Main_getPendingInterrupt(UART1));
}

extern "C" void UART2_IRQHandler()
{
	auto interrupt = DL_UART_Main_getPendingInterrupt(UART2);
	DL_UART_clearInterruptStatus(UART2, interrupt);
	UART::Interrupt(UART::UARTId::UART2, DL_UART_Main_getPendingInterrupt(UART2));
}

extern "C" void UART3_IRQHandler()
{
	auto interrupt = DL_UART_Main_getPendingInterrupt(UART3);
	DL_UART_clearInterruptStatus(UART3, interrupt);
	UART::Interrupt(UART::UARTId::UART3, DL_UART_Main_getPendingInterrupt(UART3));
}

UART::UART(UARTId id, DMAChannal dmaChannalRx, unsigned short bufferRxSize, unsigned short bufferRxCount, DMAChannal dmaChannalTx) : id{ id }
{
	if (dmaChannalRx == DMAChannal::None ||
		dmaChannalTx == DMAChannal::None)
	{
		*((volatile int*)nullptr) = 0xFF00FF00;
		//throw "not support now";
	}

	if (UART::bufferRx[id] != nullptr)
	{
		for (unsigned short i = 0; i < UART::bufferRxCount[id]; i++)
			delete[] UART::bufferRx[id][i].buffer;

		delete[] UART::bufferRx[id];
	}

	UART::bufferRxCount[id] = bufferRxCount;
	UART::bufferRxWriteIndex[id] = 0;
	UART::bufferRxReadIndex[id] = 0;
	UART::bufferRx[id] = new Buffer[bufferRxCount];

	for (unsigned short i = 0; i < bufferRxCount; i++)
	{
		UART::bufferRx[id][i].bufferDataSize = 0;
		UART::bufferRx[id][i].bufferMemSize = bufferRxSize;
		UART::bufferRx[id][i].buffer = new char[bufferRxSize];
	}

	UART::dmaChannalRx[id] = dmaChannalRx;
	UART::dmaChannalTx[id] = dmaChannalTx;

	auto& rxBuffer = UART::bufferRx[id][0];

	DL_DMA_setDestAddr(DMA, dmaChannalRx, (uint32_t)rxBuffer.buffer);
	DL_DMA_setSrcAddr(DMA, dmaChannalRx, (uint32_t) & (uartInctance[id]->RXDATA));
	DL_DMA_setTransferSize(DMA, dmaChannalRx, rxBuffer.bufferMemSize);
	DL_DMA_enableChannel(DMA, dmaChannalRx);
}

unsigned short UART::getReceiveCount()
{
	return UART::bufferRx[id][UART::bufferRxReadIndex[id]].bufferDataSize;
}

unsigned short UART::getReceive(void* buffer, unsigned short maxCount)
{
	auto& readBufer = bufferRx[id][bufferRxReadIndex[id]];

	bufferRxReadIndex[id]++;
	bufferRxReadIndex[id] %= bufferRxCount[id];

	if (maxCount > readBufer.bufferDataSize)
		maxCount = readBufer.bufferDataSize;
	memcpy(buffer, readBufer.buffer, maxCount);
	readBufer.bufferDataSize = 0;
	return maxCount;
}

bool UART::isTransiting()
{
	return DL_DMA_isChannelEnabled(DMA, dmaChannalTx[id]);
}

bool UART::transit(const void* buffer, unsigned short count)
{
	if (isTransiting()) return false;
	DL_DMA_setSrcAddr(DMA, UART::dmaChannalTx[id], (uint32_t)buffer);
	DL_DMA_setDestAddr(DMA, UART::dmaChannalTx[id], (uint32_t)(uint32_t) & (uartInctance[id]->TXDATA));
	DL_DMA_setTransferSize(DMA, UART::dmaChannalTx[id], count);
	DL_DMA_enableChannel(DMA, UART::dmaChannalTx[id]);
	return true;
}

void UART::Interrupt(UARTId uartId, DL_UART_IIDX event)
{
	switch (event)
	{
	case DL_UART_MAIN_IIDX_RX_TIMEOUT_ERROR:
	{
		auto& writeIndex = UART::bufferRxWriteIndex[uartId];
		auto& rxBufferOld = UART::bufferRx[uartId][writeIndex];

		writeIndex++;
		writeIndex %= UART::bufferRxCount[uartId];

		auto& rxBuffer = UART::bufferRx[uartId][writeIndex];

		unsigned short size = DL_DMA_getTransferSize(DMA, UART::dmaChannalRx[uartId]);

		DL_DMA_disableChannel(DMA, UART::dmaChannalRx[uartId]);
		DL_DMA_setDestAddr(DMA, UART::dmaChannalRx[uartId], (uint32_t)rxBuffer.buffer);
		DL_DMA_setTransferSize(DMA, UART::dmaChannalRx[uartId], rxBuffer.bufferMemSize);
		DL_DMA_enableChannel(DMA, UART::dmaChannalRx[uartId]);

		unsigned short count = rxBufferOld.bufferMemSize - size;
		while (!DL_UART_isRXFIFOEmpty(uartInctance[uartId]))
			rxBufferOld.buffer[count++] = DL_UART_receiveData(uartInctance[uartId]);

		rxBufferOld.bufferDataSize = count;
		break;
	}
	case DL_UART_MAIN_IIDX_DMA_DONE_RX:
	{
		auto& writeIndex = UART::bufferRxWriteIndex[uartId];
		auto& rxBufferOld = UART::bufferRx[uartId][writeIndex];

		writeIndex++;
		writeIndex %= UART::bufferRxCount[uartId];

		auto& rxBuffer = UART::bufferRx[uartId][writeIndex];

		DL_DMA_setDestAddr(DMA, UART::dmaChannalRx[uartId], (uint32_t)rxBuffer.buffer);
		DL_DMA_setTransferSize(DMA, UART::dmaChannalRx[uartId], rxBuffer.bufferMemSize);
		DL_DMA_enableChannel(DMA, UART::dmaChannalRx[uartId]);

		rxBufferOld.bufferDataSize = rxBufferOld.bufferMemSize;
		break;
	}
	case DL_UART_MAIN_IIDX_DMA_DONE_TX:
	default:
	{
		break;
	}
	}
}

