#include "ti_msp_dl_config.h"

int main()
{
	SYSCFG_DL_init();

	void test();

	int a = 0;
	while (true)
	{
		test();
		a++;
	}
}
