#pragma once

template <class T>
class SmartNew
{
public:
	SmartNew(size_t size = 1) { p = new T[size]; }
	~SmartNew() { delete[] p; p = nullptr; }

	T* operator->() { return p; }
	operator T* () { return p; }
	SmartNew<T>& operator=(T* np) { p = np; return *this; }
	SmartNew<T>& operator=(SmartNew<T>& cp) = delete;
	SmartNew<T>& operator=(SmartNew<T>&& cp) { T old = p; p = cp.p; cp.p = old; return *this; }

	T* p = nullptr;
};
