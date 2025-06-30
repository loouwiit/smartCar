#include "ti_msp_dl_config.h"

void test()
{
	DL_GPIO_togglePins(LED_PORT, LED_R_PIN);
	delay_cycles(CPUCLK_FREQ);
	DL_GPIO_togglePins(LED_PORT, LED_R_PIN);
	delay_cycles(CPUCLK_FREQ);
	DL_GPIO_togglePins(LED_PORT, LED_G_PIN);
	delay_cycles(CPUCLK_FREQ);
	DL_GPIO_togglePins(LED_PORT, LED_G_PIN);
	delay_cycles(CPUCLK_FREQ);
	DL_GPIO_togglePins(LED_PORT, LED_B_PIN);
	delay_cycles(CPUCLK_FREQ);
	DL_GPIO_togglePins(LED_PORT, LED_B_PIN);
	delay_cycles(CPUCLK_FREQ);
}
