/***************************************************************************//**
* \file main.c
* \version 1.0
*
* \brief
* Objective:
*    Starts the Cortex-M4 and enters sleep.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_device_headers.h"

int main(void)
{
    /* enable interrupts, and the CM4 */
    __enable_irq();
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);

    for (;;)
    {
        __WFI();
    }
}
