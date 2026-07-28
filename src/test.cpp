#include "ti_msp_dl_config.h"

void test()
{
	DL_GPIO_togglePins(PG_PORT, PG_PP_PIN);
	delay_cycles(CPUCLK_FREQ);
	DL_GPIO_togglePins(PG_PORT, PG_PP_PIN);
	delay_cycles(CPUCLK_FREQ);
}
