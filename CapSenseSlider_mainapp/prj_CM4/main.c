/***************************************************************************//**
* \file main.c
* \version 1.0
*
* \brief
* Objective:
*    This code example demonstrates the following functionality of the CapSense
*    component:
*        - 5-segment Linear Slider operation;
*        - CSX button operation;
*        - Run-time tuning;
*
* User interface:
*    By default the user interface consists from the following items:
*        - 5-segment Linear Slider (SLIDER), configured for operation in CSD
*          sensing mode;
*        - 1-Button (BTN0), configured to operate in CSX sensing mode;
*        - RGB LED;
*
*    The 5-segment Linear Slider (SLIDER) is intended to change brightness of the
*    Blue part of RGB LED;
*    The CSX Button (BTN0) is intended to toggle state of the Green part of
*    RGB LED;
*
* Compatible Kits:
*    CY8CKIT-062-BLE
*    CY8CKIT-062-WIFI-BT
*
* Instructions:
*
*    To Launch the CapSense Tuner tool:
*        1. Double click on design.modus item in Project Workspace to open the
*           Modus Configurator;
*        2. Switch to the Peripherals tab and select the CSD (CapSense) item;
*        3. Click the Launch CapSense Tuner item in the Parameters window;
*        4. Perform the Tools->Tuner Communication Setup command;
*        5. Select the I2C port and press OK;
*        6. Press Connect in CapSense Tuner;
*        7. Press Start in CapSense Tuner;
*
*    To change Widget parameter by using Tuner GUI:
*        1. Switch to the WidgetView tab;
*        2. Select widget in the Widget Explorer window;
*        3. Change parameter in the Widget/Sensor Parameters window;
*        4. Press the Apply to Device button to write changes to the target
*           device memory;
*
*    To add second CSX Button (BTN1):
*        1. Stop, disconnect, and close the CapSense Tuner;
*        2. Double click on design.modus item in Project Workspace to open the
*           Modus Configurator;
*        3. Switch to the Peripherals tab and select the CSD (CapSense) item;
*        4. Click the Launch CapSense Configurator item in the Parameters window;
*        5. Switch to the Basic tab and increment number of Rx electrodes
*           for Button0;
*        6. Switch to the Pins tab and assign Button0_Rx1 electrode to P8[2];
*        7. Press the Save button and close the CapSense Configurator;
*        8. Perform the File->Save command in the Modus Configurator and close it;
*        9. Uncomment the CHECK_BTN1_STATE define in the main.c file to enable
*           control of Red part of RGB LED by BTN1;
*       10. Repeat steps 2-3 from the "To Program and Run the project" section;
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "cy_device_headers.h"
#include "cy_sysclk.h"
#include "cy_sysint.h"
#include "cy_scb_ezi2c.h"
#include "cy_capsense.h"
#include "cycfg_peripherals.h"
#include "cycfg_platform.h"
#include "cycfg_pins.h"


/***************************************************************************
* Global constants
***************************************************************************/
#define EZI2C_HW                        (SCB3)

#define SLIDER_POS_TO_COMPARE_SCALER    (320u)

#define MY_TCPWM_PWM_NUM   (1UL)
#define MY_TCPWM_PWM_MASK  (1UL << MY_TCPWM_PWM_NUM)


#define CHECK_BTN0_STATE    (1uL)
//#define CHECK_BTN1_STATE  (1uL)
#define CHECK_SLDR_STATE    (1uL)


/*******************************************************************************
* Internal types
*******************************************************************************/
typedef enum{LED_OFF = 1u, LED_ON = 0u} LED_STATE;


/***************************************
* Function Prototypes
**************************************/
void UpdateLEDs(void);
void InitTunerCommunication(void);
void InitLedIndication(void);


/*******************************************************************************
* Interrupt configuration
*******************************************************************************/
static void EZI2C_Interrupt(void);

const cy_stc_sysint_t CapSense_ISR_cfg =
{
    .intrSrc = csd_interrupt_IRQn,
    .intrPriority = 7u,
};

const cy_stc_sysint_t EZI2C_ISR_cfg = {
    .intrSrc = scb_3_interrupt_IRQn,
    .intrPriority = 7u
};


/*******************************************************************************
* Global variables
*******************************************************************************/
cy_stc_scb_ezi2c_context_t EZI2C_context;
cy_stc_capsense_context_t capsenseContext = {0uL};


int main(void)
{
    /* Enable interrupts */
    __enable_irq();

    InitLedIndication();
    InitTunerCommunication();

    /* Start CapSense block - Initializes CapSense Data structure and
       performs first scan to set up sensor baselines */
    Cy_CapSense_Start(&capsenseContext);

    for (;;)
    {
        if(CY_CAPSENSE_NOT_BUSY == Cy_CapSense_IsBusy(&capsenseContext))
        {
            /* Process all widgets */
            Cy_CapSense_ProcessAllWidgets(&capsenseContext);

            /* Communication with the Tuner application. */
            Cy_CapSense_RunTuner(&capsenseContext);

            /* Display CapSense widgets state by using LEDs */
            UpdateLEDs();

            /* Start next scan */
            Cy_CapSense_ScanAllWidgets(&capsenseContext);
        }
    }
}


/*******************************************************************************
* Function Name: InitTunerCommunication
****************************************************************************//**
*
* \brief
*   Initializes the Tuner Communication interface.
*
* \details
*   This function covers the following functionality:
*       - Initializes SCB block for operation in EZI2C mode;
*       - Connects EZI2C HW to the SDA and SCL pins;
*       - Sets the SDA and SCL pins drive mode to the "Open drain, drives low" mode;
*       - Sets communication data buffer to CapSense data structure;
*
*******************************************************************************/
void InitTunerCommunication(void)
{
    /* Connect EZI2C HW to the SDA and SCL pins. */
    Cy_GPIO_Pin_FastInit(EZI2C_SCL_PORT, EZI2C_SCL_PIN, CY_GPIO_DM_OD_DRIVESLOW, 1u, EZI2C_SCL_HSIOM);
    Cy_GPIO_Pin_FastInit(EZI2C_SDA_PORT, EZI2C_SDA_PIN, CY_GPIO_DM_OD_DRIVESLOW, 1u, EZI2C_SDA_HSIOM);
    Cy_SCB_EZI2C_Init(EZI2C_HW, &EZI2C_cfg, &EZI2C_context);

    /* Initialize and enable EZI2C interrupts */
    Cy_SysInt_Init(&EZI2C_ISR_cfg, &EZI2C_Interrupt);
    NVIC_EnableIRQ(EZI2C_ISR_cfg.intrSrc);

    /* Set up communication data buffer to CapSense data structure to be exposed to
    I2C master at primary slave address request. */
    Cy_SCB_EZI2C_SetBuffer1(EZI2C_HW, (uint8 *)&cy_capsense_dsRam,
        sizeof(cy_capsense_dsRam), sizeof(cy_capsense_dsRam), &EZI2C_context);

    /* Enable EZI2C block */
    Cy_SCB_EZI2C_Enable(EZI2C_HW);
}


/*******************************************************************************
* Function Name: InitLedIndication
****************************************************************************//**
*
* \brief
*   Initializes peripherals, assigned to indicate CapSense by RGB LED.
*
* \details
*   This function covers the following functionality:
*       - Initialize pins, assigned to drive RGB LED;
*       - Initialize the TCPWM HW block to operate in PWM mode to control
*         brightness of blue LED;
*       - Sets the SDA and SCL pins drive mode to the "Open drain, drives low" mode;
*       - Sets communication data buffer to CapSense data structure;
*
*******************************************************************************/
void InitLedIndication(void)
{
    /* Initialize pins, assigned to drive RGB LED. */
    Cy_GPIO_Pin_Init(Red_PORT, Red_PIN, &Red_cfg);
    Cy_GPIO_Pin_Init(Green_PORT, Green_PIN, &Green_cfg);
    Cy_GPIO_Pin_Init(Blue_PORT, Blue_PIN, &Blue_cfg);

    /* Initialize the TCPWM HW block to operate in PWM mode. This block is used to control brightness of blue LED. */
    if (CY_TCPWM_SUCCESS != Cy_TCPWM_PWM_Init(TCPWM0, MY_TCPWM_PWM_NUM, &tcpwm_0__cnt_1__cfg))
    {
        /* Handle possible errors */
    }

    /* Enable the initialized PWM */
    Cy_TCPWM_PWM_Enable(TCPWM0, MY_TCPWM_PWM_NUM);

    /* Then start the PWM */
    Cy_TCPWM_TriggerStart(TCPWM0, MY_TCPWM_PWM_MASK);
}


/*******************************************************************************
* Function Name: UpdateLEDs
****************************************************************************//**
*
* \brief
*   Updates state of RGB LED according to status of CapSense widgets.
*
* \details
*   This function covers the following functionality:
*       - Toggles the state of the Green LED when BTN0 is touched;
*       - Toggles the state of the Red LED when BTN1 is touched;
*       - Changes the brightness of the Blue LED depending of position
*         of touch on the Slider;
*
*******************************************************************************/
void UpdateLEDs(void)
{
    static uint16_t curPos  = 0u;
    static uint16_t prevPos = 0u;

    static uint8 btn0CurState  = 0u;
    static uint8 btn0PrevState = 0u;
    static uint8 btn1CurState  = 0u;
    static uint8 btn1PrevState = 0u;

    static uint8 greenLedState = LED_OFF;
    static uint8 redLedState   = LED_OFF;


    #if defined(CHECK_SLDR_STATE)

        /* Check Slider for detected touches. */
        if(0uL != Cy_CapSense_IsWidgetActive(CY_CAPSENSE_LINEARSLIDER0_WDGT_ID, &capsenseContext))
        {
            /* Get current touch position. */
            curPos  = Cy_CapSense_GetCentroidPos(CY_CAPSENSE_LINEARSLIDER0_WDGT_ID, &capsenseContext);

            if(CY_CAPSENSE_SLIDER_NO_TOUCH == curPos)
            {
                curPos = 0u;
            }
        }
    #endif

    /* Move bargraph */
    if (curPos != prevPos)
    {
        prevPos = curPos;
        /* Display Slider bargraph */
        if (curPos != 0u)
        {
            /* Update Blue LED brightness based on the current touch position value. */
            Cy_TCPWM_PWM_SetCompare0(TCPWM0, MY_TCPWM_PWM_NUM, ((uint32)curPos * SLIDER_POS_TO_COMPARE_SCALER));
        }
    }

    #if defined(CHECK_BTN0_STATE)
        /* Get current state of BTN0. */
        btn0CurState = Cy_CapSense_IsSensorActive(CY_CAPSENSE_BUTTON0_WDGT_ID, CY_CAPSENSE_BUTTON0_SNS0_ID, &capsenseContext);
    #endif

    #if(CHECK_BTN1_STATE)
        /* Get current state of BTN1. */
        btn1CurState = Cy_CapSense_IsSensorActive(CY_CAPSENSE_BUTTON0_WDGT_ID, CY_CAPSENSE_BUTTON0_SNS1_ID, &capsenseContext);
    #endif

    /* Toggle state of the Red LED if new touch to the BTN0 is detected. */
    if((0u != btn0CurState) && (0u == btn0PrevState))
    {
        greenLedState = (LED_OFF == greenLedState) ? LED_ON : LED_OFF;
        btn0PrevState = btn0CurState;
    }
    else
    {
        btn0PrevState = btn0CurState;
    }

    /* Toggle state of the Red LED if new touch to the BTN1 is detected. */
    if((0u != btn1CurState) && (0u == btn1PrevState))
    {
        redLedState = (LED_OFF == redLedState) ? LED_ON : LED_OFF;
        btn1PrevState = btn1CurState;
    }
    else
    {
        btn1PrevState = btn1CurState;
    }

    Cy_GPIO_Write(Green_PORT, Green_PIN, greenLedState);
    Cy_GPIO_Write(Red_PORT,   Red_PIN,   redLedState);
}


/*******************************************************************************
* Function Name: EZI2C_Interrupt
****************************************************************************//**
*
* \brief
*   EZI2C interrupt handler.
*
*******************************************************************************/
static void EZI2C_Interrupt(void)
{
    Cy_SCB_EZI2C_Interrupt(EZI2C_HW, &EZI2C_context);
}
