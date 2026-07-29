#pragma once

template <class T, unsigned size = 2>
class Mixer
{
public:
	using Function_t = void (*)();

	// []**不**会处理mix
	T& operator [](unsigned index) { return in[index]; }

	// mix会自动处理callback
	void setMixCallback(Function_t callback) { if (callback == nullptr) this->callback = emptyCallback; else this->callback = callback; }

	// mix会自动处理callback
	Mixer& mix() { out = 0; for (unsigned i = 0;i < size;i++) if (disableCount[i] == 0) out += in[i]; callback(); return *this; }

	// disable会处理mix
	void disable(unsigned index) { disableCount[index]++; if (disableCount[index] != 0) mix(); }

	// enable会处理mix
	void enable(unsigned index) { disableCount[index]--; if (disableCount[index] == 0) mix(); }

	unsigned char isDisable(unsigned index) { return disableCount[index]; }

	operator T() { return out; }
private:
	static void emptyCallback() {}

	Function_t callback = emptyCallback;
	T in[size];
	unsigned char disableCount[size]{};
	T out;
};

class MixNumber
{
public:
	enum : unsigned char {
		Uart,
		GraySensor,
		Key,
		Turn,

		Count,
	};
};
