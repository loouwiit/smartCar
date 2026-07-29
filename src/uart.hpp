#pragma once

#include <ti/driverlib/dl_uart_main.h>

extern "C" void UART0_IRQHandler();
extern "C" void UART1_IRQHandler();
extern "C" void UART2_IRQHandler();
extern "C" void UART3_IRQHandler();

class UART
{
public:
	class UARTInctance
	{
	public:
		UART_Regs* uart = None;
		UARTInctance(UART_Regs* const uart) { this->uart = uart; }

		operator UART_Regs* () { return uart; }
		UART_Regs* operator->() { return uart; }

		constexpr static UART_Regs* None = nullptr;
	};

	class UARTId
	{
	public:
		unsigned char id = None;

		UARTId(unsigned char id = None) : id{ id } {}
		UARTId& operator=(UARTId&) = default;
		UARTId(UARTId&) = default;

		UARTId(const UARTId& copy) { id = copy.id; }
		UARTId& operator=(const UARTId& copy) { id = copy.id; return *this; }

		UARTId(UARTInctance inctance)
		{
			switch ((unsigned int)inctance.uart)
			{
			case UART0_BASE: id = UART0; break;
			case UART1_BASE: id = UART1; break;
			case UART2_BASE: id = UART2; break;
			case UART3_BASE: id = UART3; break;
			default: id = None; break;
			}
		};

		operator unsigned char() { return id; }

		constexpr static unsigned char None = -1;
		constexpr static unsigned char UART0 = 0;
		constexpr static unsigned char UART1 = 1;
		constexpr static unsigned char UART2 = 2;
		constexpr static unsigned char UART3 = 3;
	};

	class DMAChannal
	{
	public:
		unsigned char channal = None;

		DMAChannal(unsigned char channal = None) : channal{ channal } {};
		DMAChannal(DMAChannal&) = default;
		DMAChannal& operator=(DMAChannal&) = default;
		operator unsigned char() { return channal; }

		constexpr static unsigned char None = -1;
		constexpr static unsigned char Channal0 = 0;
		constexpr static unsigned char Channal1 = 1;
		constexpr static unsigned char Channal2 = 2;
		constexpr static unsigned char Channal3 = 3;
		constexpr static unsigned char Channal4 = 4;
		constexpr static unsigned char Channal5 = 5;
		constexpr static unsigned char Channal6 = 6;
		constexpr static unsigned char Channal7 = 7;
	};

	UART() = default;
	UART(UARTId id, DMAChannal dmaChannalRx, unsigned short bufferRxSize, unsigned short bufferRxCount, DMAChannal dmaChannalTx);

	unsigned short getReceiveCount();
	unsigned short getReceive(void* buffer, unsigned short maxCount);
	bool isTransiting();
	bool transit(const void* buffer, unsigned short count);

	UARTId id = UARTId::None;

private:
	class Buffer
	{
	public:
		char* buffer = nullptr;
		unsigned short bufferMemSize = 0;
		unsigned short bufferDataSize = 0;
	};

	static UARTInctance uartInctance[4];
	static DMAChannal dmaChannalRx[4];
	static DMAChannal dmaChannalTx[4];
	static Buffer* bufferRx[4];
	static unsigned short bufferRxCount[4];
	static unsigned short bufferRxWriteIndex[4];
	static unsigned short bufferRxReadIndex[4];

	friend void UART0_IRQHandler();
	friend void UART1_IRQHandler();
	friend void UART2_IRQHandler();
	friend void UART3_IRQHandler();

	static void Interrupt(UARTId uartInstance, DL_UART_IIDX event);
};
