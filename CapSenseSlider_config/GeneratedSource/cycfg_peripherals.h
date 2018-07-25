/*******************************************************************************
* File Name: cycfg_peripherals.h
*
* Description:
* Peripheral Hardware Block configuration
* This file should not be modified. It was automatically generated by 
* ModusToolbox 1.0.0
* 
********************************************************************************
* Copyright (c) 2017-2018 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CYCFG_PERIPHERALS_H)
#define CYCFG_PERIPHERALS_H

#include "notices.h"
#include "cy_sysclk.h"
#include "cy_scb_ezi2c.h"
#include "cy_tcpwm_pwm.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CY_CAPSENSE_CPU_CLK_HZ (50000000u)
#define CY_CAPSENSE_PERI_CLK_TYPE (CY_SYSCLK_DIV_8_BIT)
#define CY_CAPSENSE_PERI_CLK_IDX (0u)

extern const cy_stc_scb_ezi2c_config_t EZI2C_cfg;
extern const cy_stc_tcpwm_pwm_config_t tcpwm_0__cnt_1__cfg;

void init_cycfg_peripherals(void);

#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PERIPHERALS_H */
