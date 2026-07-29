/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  2500
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for Motor_Pwm */
#define Motor_Pwm_INST                                                     TIMA1
#define Motor_Pwm_INST_IRQHandler                               TIMA1_IRQHandler
#define Motor_Pwm_INST_INT_IRQN                                 (TIMA1_INT_IRQn)
#define Motor_Pwm_INST_CLK_FREQ                                          1000000
/* GPIO defines for channel 0 */
#define GPIO_Motor_Pwm_C0_PORT                                             GPIOA
#define GPIO_Motor_Pwm_C0_PIN                                     DL_GPIO_PIN_15
#define GPIO_Motor_Pwm_C0_IOMUX                                  (IOMUX_PINCM37)
#define GPIO_Motor_Pwm_C0_IOMUX_FUNC                 IOMUX_PINCM37_PF_TIMA1_CCP0
#define GPIO_Motor_Pwm_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_Motor_Pwm_C1_PORT                                             GPIOA
#define GPIO_Motor_Pwm_C1_PIN                                     DL_GPIO_PIN_16
#define GPIO_Motor_Pwm_C1_IOMUX                                  (IOMUX_PINCM38)
#define GPIO_Motor_Pwm_C1_IOMUX_FUNC                 IOMUX_PINCM38_PF_TIMA1_CCP1
#define GPIO_Motor_Pwm_C1_IDX                                DL_TIMER_CC_1_INDEX

/* Defines for Servomotor_Pwm */
#define Servomotor_Pwm_INST                                                TIMG6
#define Servomotor_Pwm_INST_IRQHandler                          TIMG6_IRQHandler
#define Servomotor_Pwm_INST_INT_IRQN                            (TIMG6_INT_IRQn)
#define Servomotor_Pwm_INST_CLK_FREQ                                     1000000
/* GPIO defines for channel 1 */
#define GPIO_Servomotor_Pwm_C1_PORT                                        GPIOB
#define GPIO_Servomotor_Pwm_C1_PIN                                 DL_GPIO_PIN_7
#define GPIO_Servomotor_Pwm_C1_IOMUX                             (IOMUX_PINCM24)
#define GPIO_Servomotor_Pwm_C1_IOMUX_FUNC             IOMUX_PINCM24_PF_TIMG6_CCP1
#define GPIO_Servomotor_Pwm_C1_IDX                           DL_TIMER_CC_1_INDEX



/* Defines for EncoderCapture */
#define EncoderCapture_INST                                              (TIMA0)
#define EncoderCapture_INST_IRQHandler                          TIMA0_IRQHandler
#define EncoderCapture_INST_INT_IRQN                            (TIMA0_INT_IRQn)
#define EncoderCapture_INST_LOAD_VALUE                                  (49999U)
/* GPIO defines for channel 0 */
#define GPIO_EncoderCapture_C0_PORT                                        GPIOA
#define GPIO_EncoderCapture_C0_PIN                                 DL_GPIO_PIN_8
#define GPIO_EncoderCapture_C0_IOMUX                             (IOMUX_PINCM19)
#define GPIO_EncoderCapture_C0_IOMUX_FUNC             IOMUX_PINCM19_PF_TIMA0_CCP0
/* GPIO defines for channel 1 */
#define GPIO_EncoderCapture_C1_PORT                                        GPIOA
#define GPIO_EncoderCapture_C1_PIN                                DL_GPIO_PIN_22
#define GPIO_EncoderCapture_C1_IOMUX                             (IOMUX_PINCM47)
#define GPIO_EncoderCapture_C1_IOMUX_FUNC             IOMUX_PINCM47_PF_TIMA0_CCP1






/* Defines for I2C_OLED */
#define I2C_OLED_INST                                                       I2C1
#define I2C_OLED_INST_IRQHandler                                 I2C1_IRQHandler
#define I2C_OLED_INST_INT_IRQN                                     I2C1_INT_IRQn
#define I2C_OLED_BUS_SPEED_HZ                                             400000
#define GPIO_I2C_OLED_SDA_PORT                                             GPIOB
#define GPIO_I2C_OLED_SDA_PIN                                      DL_GPIO_PIN_3
#define GPIO_I2C_OLED_IOMUX_SDA                                  (IOMUX_PINCM16)
#define GPIO_I2C_OLED_IOMUX_SDA_FUNC                   IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_I2C_OLED_SCL_PORT                                             GPIOB
#define GPIO_I2C_OLED_SCL_PIN                                      DL_GPIO_PIN_2
#define GPIO_I2C_OLED_IOMUX_SCL                                  (IOMUX_PINCM15)
#define GPIO_I2C_OLED_IOMUX_SCL_FUNC                   IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           40000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_0_FBRD_40_MHZ_115200_BAUD                                      (45)





/* Defines for GraySensorAdc */
#define GraySensorAdc_INST                                                  ADC1
#define GraySensorAdc_INST_IRQHandler                            ADC1_IRQHandler
#define GraySensorAdc_INST_INT_IRQN                              (ADC1_INT_IRQn)
#define GraySensorAdc_ADCMEM_Cha                              DL_ADC12_MEM_IDX_0
#define GraySensorAdc_ADCMEM_Cha_REF             DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define GraySensorAdc_ADCMEM_Cha_REF_VOLTAGE_V                                     3.3
#define GPIO_GraySensorAdc_C2_PORT                                         GPIOA
#define GPIO_GraySensorAdc_C2_PIN                                 DL_GPIO_PIN_17
#define GPIO_GraySensorAdc_IOMUX_C2                              (IOMUX_PINCM39)
#define GPIO_GraySensorAdc_IOMUX_C2_FUNC          (IOMUX_PINCM39_PF_UNCONNECTED)



/* Defines for DMA_RX */
#define DMA_RX_CHAN_ID                                                       (1)
#define UART_0_INST_DMA_TRIGGER_0                            (DMA_UART0_RX_TRIG)
/* Defines for DMA_TX */
#define DMA_TX_CHAN_ID                                                       (0)
#define UART_0_INST_DMA_TRIGGER_1                            (DMA_UART0_TX_TRIG)


/* Port definition for Pin Group GPIO_MPU6050 */
#define GPIO_MPU6050_PORT                                                (GPIOB)

/* Defines for PIN_INT: GPIOB.1 with pinCMx 13 on package pin 48 */
// groups represented: ["KEY","GPIO_MPU6050"]
// pins affected: ["KEY_1","PIN_INT"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_MPU6050_PIN_INT_IIDX                            (DL_GPIO_IIDX_DIO1)
#define GPIO_MPU6050_PIN_INT_PIN                                 (DL_GPIO_PIN_1)
#define GPIO_MPU6050_PIN_INT_IOMUX                               (IOMUX_PINCM13)
/* Port definition for Pin Group KEY */
#define KEY_PORT                                                         (GPIOB)

/* Defines for KEY_1: GPIOB.21 with pinCMx 49 on package pin 20 */
#define KEY_KEY_1_IIDX                                      (DL_GPIO_IIDX_DIO21)
#define KEY_KEY_1_PIN                                           (DL_GPIO_PIN_21)
#define KEY_KEY_1_IOMUX                                          (IOMUX_PINCM49)
/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for G: GPIOB.27 with pinCMx 58 on package pin 29 */
#define LED_G_PIN                                               (DL_GPIO_PIN_27)
#define LED_G_IOMUX                                              (IOMUX_PINCM58)
/* Defines for A1: GPIOB.24 with pinCMx 52 on package pin 23 */
#define Motor_A1_PORT                                                    (GPIOB)
#define Motor_A1_PIN                                            (DL_GPIO_PIN_24)
#define Motor_A1_IOMUX                                           (IOMUX_PINCM52)
/* Defines for A2: GPIOA.18 with pinCMx 40 on package pin 11 */
#define Motor_A2_PORT                                                    (GPIOA)
#define Motor_A2_PIN                                            (DL_GPIO_PIN_18)
#define Motor_A2_IOMUX                                           (IOMUX_PINCM40)
/* Defines for B1: GPIOB.9 with pinCMx 26 on package pin 61 */
#define Motor_B1_PORT                                                    (GPIOB)
#define Motor_B1_PIN                                             (DL_GPIO_PIN_9)
#define Motor_B1_IOMUX                                           (IOMUX_PINCM26)
/* Defines for B2: GPIOA.24 with pinCMx 54 on package pin 25 */
#define Motor_B2_PORT                                                    (GPIOA)
#define Motor_B2_PIN                                            (DL_GPIO_PIN_24)
#define Motor_B2_IOMUX                                           (IOMUX_PINCM54)
/* Defines for GraySensorA: GPIOB.15 with pinCMx 32 on package pin 3 */
#define GraySensor_GraySensorA_PORT                                      (GPIOB)
#define GraySensor_GraySensorA_PIN                              (DL_GPIO_PIN_15)
#define GraySensor_GraySensorA_IOMUX                             (IOMUX_PINCM32)
/* Defines for GraySensorB: GPIOA.12 with pinCMx 34 on package pin 5 */
#define GraySensor_GraySensorB_PORT                                      (GPIOA)
#define GraySensor_GraySensorB_PIN                              (DL_GPIO_PIN_12)
#define GraySensor_GraySensorB_IOMUX                             (IOMUX_PINCM34)
/* Defines for GraySensorC: GPIOB.16 with pinCMx 33 on package pin 4 */
#define GraySensor_GraySensorC_PORT                                      (GPIOB)
#define GraySensor_GraySensorC_PIN                              (DL_GPIO_PIN_16)
#define GraySensor_GraySensorC_IOMUX                             (IOMUX_PINCM33)
/* Defines for EncoderGpioA: GPIOB.19 with pinCMx 45 on package pin 16 */
#define EncoderGpio_EncoderGpioA_PORT                                    (GPIOB)
#define EncoderGpio_EncoderGpioA_PIN                            (DL_GPIO_PIN_19)
#define EncoderGpio_EncoderGpioA_IOMUX                           (IOMUX_PINCM45)
/* Defines for EncoderGpioB: GPIOA.25 with pinCMx 55 on package pin 26 */
#define EncoderGpio_EncoderGpioB_PORT                                    (GPIOA)
#define EncoderGpio_EncoderGpioB_PIN                            (DL_GPIO_PIN_25)
#define EncoderGpio_EncoderGpioB_IOMUX                           (IOMUX_PINCM55)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_Motor_Pwm_init(void);
void SYSCFG_DL_Servomotor_Pwm_init(void);
void SYSCFG_DL_EncoderCapture_init(void);
void SYSCFG_DL_I2C_OLED_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_GraySensorAdc_init(void);
void SYSCFG_DL_DMA_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
