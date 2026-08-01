#pragma once

template <class T = float>
class Filter
{
public:
	float alpha = 0.75f;
	T value{};

	auto operator()(T newValue)
	{
		value = value * (1 - alpha) + newValue * alpha;
		return value;
	}

	operator T()
	{
		return value;
	}
};
