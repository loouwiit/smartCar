class StackGuard
{
public:
	StackGuard()
	{
		pattern = 0xABCD1234;
	}

	~StackGuard()
	{
		if (pattern != 0xABCD1234)
		{
			if (pattern == 0xABCD1234)
				return;
			while (true)
			{
			};
		}
	}

private:
	volatile unsigned int pattern;
};
