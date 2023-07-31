/*******************************************************************************
* File Name: oled.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  The actual protocol and operation code resides in the interrupt service
*  routine file.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "oled_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 oled_initVar = 0u; /* Defines if component was initialized */

volatile uint8 oled_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: oled_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void oled_Init(void) 
{
#if (oled_FF_IMPLEMENTED)
    /* Configure fixed function block */
    oled_CFG_REG  = oled_DEFAULT_CFG;
    oled_XCFG_REG = oled_DEFAULT_XCFG;
    oled_ADDR_REG = oled_DEFAULT_ADDR;
    oled_CLKDIV1_REG = LO8(oled_DEFAULT_DIVIDE_FACTOR);
    oled_CLKDIV2_REG = HI8(oled_DEFAULT_DIVIDE_FACTOR);

#else
    uint8 intState;

    /* Configure control and interrupt sources */
    oled_CFG_REG      = oled_DEFAULT_CFG;
    oled_INT_MASK_REG = oled_DEFAULT_INT_MASK;

    /* Enable interrupt generation in status */
    intState = CyEnterCriticalSection();
    oled_INT_ENABLE_REG |= oled_INTR_ENABLE;
    CyExitCriticalSection(intState);

    /* Configure bit counter */
    #if (oled_MODE_SLAVE_ENABLED)
        oled_PERIOD_REG = oled_DEFAULT_PERIOD;
    #endif  /* (oled_MODE_SLAVE_ENABLED) */

    /* Configure clock generator */
    #if (oled_MODE_MASTER_ENABLED)
        oled_MCLK_PRD_REG = oled_DEFAULT_MCLK_PRD;
        oled_MCLK_CMP_REG = oled_DEFAULT_MCLK_CMP;
    #endif /* (oled_MODE_MASTER_ENABLED) */
#endif /* (oled_FF_IMPLEMENTED) */

#if (oled_TIMEOUT_ENABLED)
    oled_TimeoutInit();
#endif /* (oled_TIMEOUT_ENABLED) */

    /* Configure internal interrupt */
    CyIntDisable    (oled_ISR_NUMBER);
    CyIntSetPriority(oled_ISR_NUMBER, oled_ISR_PRIORITY);
    #if (oled_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(oled_ISR_NUMBER, &oled_ISR);
    #endif /* (oled_INTERN_I2C_INTR_HANDLER) */

    /* Set FSM to default state */
    oled_state = oled_SM_IDLE;

#if (oled_MODE_SLAVE_ENABLED)
    /* Clear status and buffers index */
    oled_slStatus = 0u;
    oled_slRdBufIndex = 0u;
    oled_slWrBufIndex = 0u;

    /* Configure matched address */
    oled_SlaveSetAddress(oled_DEFAULT_ADDR);
#endif /* (oled_MODE_SLAVE_ENABLED) */

#if (oled_MODE_MASTER_ENABLED)
    /* Clear status and buffers index */
    oled_mstrStatus = 0u;
    oled_mstrRdBufIndex = 0u;
    oled_mstrWrBufIndex = 0u;
#endif /* (oled_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: oled_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
*******************************************************************************/
void oled_Enable(void) 
{
#if (oled_FF_IMPLEMENTED)
    uint8 intState;

    /* Enable power to block */
    intState = CyEnterCriticalSection();
    oled_ACT_PWRMGR_REG  |= oled_ACT_PWR_EN;
    oled_STBY_PWRMGR_REG |= oled_STBY_PWR_EN;
    CyExitCriticalSection(intState);
#else
    #if (oled_MODE_SLAVE_ENABLED)
        /* Enable bit counter */
        uint8 intState = CyEnterCriticalSection();
        oled_COUNTER_AUX_CTL_REG |= oled_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    #endif /* (oled_MODE_SLAVE_ENABLED) */

    /* Enable slave or master bits */
    oled_CFG_REG |= oled_ENABLE_MS;
#endif /* (oled_FF_IMPLEMENTED) */

#if (oled_TIMEOUT_ENABLED)
    oled_TimeoutEnable();
#endif /* (oled_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: oled_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This component automatically enables its interrupt.  If I2C is enabled !
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  oled_initVar - This variable is used to check the initial
*                             configuration, modified on the first
*                             function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void oled_Start(void) 
{
    if (0u == oled_initVar)
    {
        oled_Init();
        oled_initVar = 1u; /* Component initialized */
    }

    oled_Enable();
    oled_EnableInt();
}


/*******************************************************************************
* Function Name: oled_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void oled_Stop(void) 
{
    oled_DisableInt();

#if (oled_TIMEOUT_ENABLED)
    oled_TimeoutStop();
#endif  /* End (oled_TIMEOUT_ENABLED) */

#if (oled_FF_IMPLEMENTED)
    {
        uint8 intState;
        uint16 blockResetCycles;

        /* Store registers effected by block disable */
        oled_backup.addr    = oled_ADDR_REG;
        oled_backup.clkDiv1 = oled_CLKDIV1_REG;
        oled_backup.clkDiv2 = oled_CLKDIV2_REG;

        /* Calculate number of cycles to reset block */
        blockResetCycles = ((uint16) ((uint16) oled_CLKDIV2_REG << 8u) | oled_CLKDIV1_REG) + 1u;

        /* Disable block */
        oled_CFG_REG &= (uint8) ~oled_CFG_EN_SLAVE;
        /* Wait for block reset before disable power */
        CyDelayCycles((uint32) blockResetCycles);

        /* Disable power to block */
        intState = CyEnterCriticalSection();
        oled_ACT_PWRMGR_REG  &= (uint8) ~oled_ACT_PWR_EN;
        oled_STBY_PWRMGR_REG &= (uint8) ~oled_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable block */
        oled_CFG_REG |= (uint8) oled_ENABLE_MS;

        /* Restore registers effected by block disable. Ticket ID#198004 */
        oled_ADDR_REG    = oled_backup.addr;
        oled_ADDR_REG    = oled_backup.addr;
        oled_CLKDIV1_REG = oled_backup.clkDiv1;
        oled_CLKDIV2_REG = oled_backup.clkDiv2;
    }
#else

    /* Disable slave or master bits */
    oled_CFG_REG &= (uint8) ~oled_ENABLE_MS;

#if (oled_MODE_SLAVE_ENABLED)
    {
        /* Disable bit counter */
        uint8 intState = CyEnterCriticalSection();
        oled_COUNTER_AUX_CTL_REG &= (uint8) ~oled_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    }
#endif /* (oled_MODE_SLAVE_ENABLED) */

    /* Clear interrupt source register */
    (void) oled_CSR_REG;
#endif /* (oled_FF_IMPLEMENTED) */

    /* Disable interrupt on stop (enabled by write transaction) */
    oled_DISABLE_INT_ON_STOP;
    oled_ClearPendingInt();

    /* Reset FSM to default state */
    oled_state = oled_SM_IDLE;

    /* Clear busy statuses */
#if (oled_MODE_SLAVE_ENABLED)
    oled_slStatus &= (uint8) ~(oled_SSTAT_RD_BUSY | oled_SSTAT_WR_BUSY);
#endif /* (oled_MODE_SLAVE_ENABLED) */
}


/* [] END OF FILE */
