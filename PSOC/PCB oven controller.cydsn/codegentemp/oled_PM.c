/*******************************************************************************
* File Name: oled_PM.c
* Version 3.50
*
* Description:
*  This file provides low power mode APIs for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "oled_PVT.h"

oled_BACKUP_STRUCT oled_backup =
{
    oled_DISABLE,

#if (oled_FF_IMPLEMENTED)
    oled_DEFAULT_XCFG,
    oled_DEFAULT_CFG,
    oled_DEFAULT_ADDR,
    LO8(oled_DEFAULT_DIVIDE_FACTOR),
    HI8(oled_DEFAULT_DIVIDE_FACTOR),
#else  /* (oled_UDB_IMPLEMENTED) */
    oled_DEFAULT_CFG,
#endif /* (oled_FF_IMPLEMENTED) */

#if (oled_TIMEOUT_ENABLED)
    oled_DEFAULT_TMOUT_PERIOD,
    oled_DEFAULT_TMOUT_INTR_MASK,
#endif /* (oled_TIMEOUT_ENABLED) */
};

#if ((oled_FF_IMPLEMENTED) && (oled_WAKEUP_ENABLED))
    volatile uint8 oled_wakeupSource;
#endif /* ((oled_FF_IMPLEMENTED) && (oled_WAKEUP_ENABLED)) */


/*******************************************************************************
* Function Name: oled_SaveConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Stores the component non-retention configuration registers.
*   Checked:   Disables the master, if it was enabled before, and enables
*              backup regulator of the I2C hardware. If a transaction intended
*              for component executes during this function call, it waits until
*              the current transaction is completed and I2C hardware is ready
*              to enter sleep mode. All subsequent I2C traffic is NAKed until
*              the device is put into sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  oled_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            entering the sleep mode.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void oled_SaveConfig(void) 
{
#if (oled_FF_IMPLEMENTED)
    #if (oled_WAKEUP_ENABLED)
        uint8 intState;
    #endif /* (oled_WAKEUP_ENABLED) */

    /* Store registers before enter low power mode */
    oled_backup.cfg     = oled_CFG_REG;
    oled_backup.xcfg    = oled_XCFG_REG;
    oled_backup.addr    = oled_ADDR_REG;
    oled_backup.clkDiv1 = oled_CLKDIV1_REG;
    oled_backup.clkDiv2 = oled_CLKDIV2_REG;

#if (oled_WAKEUP_ENABLED)
    /* Disable master */
    oled_CFG_REG &= (uint8) ~oled_ENABLE_MASTER;

    /* Enable backup regulator to keep block powered in low power mode */
    intState = CyEnterCriticalSection();
    oled_PWRSYS_CR1_REG |= oled_PWRSYS_CR1_I2C_REG_BACKUP;
    CyExitCriticalSection(intState);

    /* 1) Set force NACK to ignore I2C transactions;
    *  2) Wait unti I2C is ready go to Sleep; !!
    *  3) These bits are cleared on wake up.
    */
    /* Wait when block is ready for sleep */
    oled_XCFG_REG |= oled_XCFG_FORCE_NACK;
    while (0u == (oled_XCFG_REG & oled_XCFG_RDY_TO_SLEEP))
    {
    }

    /* Setup wakeup interrupt */
    oled_DisableInt();
    (void) CyIntSetVector(oled_ISR_NUMBER, &oled_WAKEUP_ISR);
    oled_wakeupSource = 0u;
    oled_EnableInt();
#endif /* (oled_WAKEUP_ENABLED) */

#else
    /* Store only address match bit */
    oled_backup.control = (oled_CFG_REG & oled_CTRL_ANY_ADDRESS_MASK);
#endif /* (oled_FF_IMPLEMENTED) */

#if (oled_TIMEOUT_ENABLED)
    oled_TimeoutSaveConfig();
#endif /* (oled_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: oled_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component before device enters
*  sleep mode. The Enable wakeup from Sleep Mode selection influences this
*  function implementation:
*   Unchecked: Checks current I2C component state, saves it, and disables the
*              component by calling I2C_Stop() if it is currently enabled.
*              I2C_SaveConfig() is then called to save the component
*              non-retention configuration registers.
*   Checked:   If a transaction intended for component executes during this
*              function call, it waits until the current transaction is
*              completed. All subsequent I2C traffic intended for component
*              is NAKed until the device is put to sleep mode. The address
*              match event wakes up the device.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void oled_Sleep(void) 
{
#if (oled_WAKEUP_ENABLED)
    /* Do not enable block after exit low power mode if it is wakeup source */
    oled_backup.enableState = oled_DISABLE;

    #if (oled_TIMEOUT_ENABLED)
        oled_TimeoutStop();
    #endif /* (oled_TIMEOUT_ENABLED) */

#else
    /* Store enable state */
    oled_backup.enableState = (uint8) oled_IS_ENABLED;

    if (0u != oled_backup.enableState)
    {
        oled_Stop();
    }
#endif /* (oled_WAKEUP_ENABLED) */

    oled_SaveConfig();
}


/*******************************************************************************
* Function Name: oled_RestoreConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              to the state they were in before I2C_Sleep() or I2C_SaveConfig()
*              was called.
*   Checked:   Disables the backup regulator of the I2C hardware. Sets up the
*              regular component interrupt handler and generates the component
*              interrupt if it was wake up source to release the bus and
*              continue in-coming I2C transaction.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  oled_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            exiting the sleep mode.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before oled_SaveConfig() or
*  oled_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void oled_RestoreConfig(void) 
{
#if (oled_FF_IMPLEMENTED)
    uint8 intState;

    if (oled_CHECK_PWRSYS_I2C_BACKUP)
    /* Low power mode was Sleep - backup regulator is enabled */
    {
        /* Enable backup regulator in active mode */
        intState = CyEnterCriticalSection();
        oled_PWRSYS_CR1_REG &= (uint8) ~oled_PWRSYS_CR1_I2C_REG_BACKUP;
        CyExitCriticalSection(intState);

        /* Restore master */
        oled_CFG_REG = oled_backup.cfg;
    }
    else
    /* Low power mode was Hibernate - backup regulator is disabled. All registers are cleared */
    {
    #if (oled_WAKEUP_ENABLED)
        /* Disable power to block before register restore */
        intState = CyEnterCriticalSection();
        oled_ACT_PWRMGR_REG  &= (uint8) ~oled_ACT_PWR_EN;
        oled_STBY_PWRMGR_REG &= (uint8) ~oled_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable component in I2C_Wakeup() after register restore */
        oled_backup.enableState = oled_ENABLE;
    #endif /* (oled_WAKEUP_ENABLED) */

        /* Restore component registers after Hibernate */
        oled_XCFG_REG    = oled_backup.xcfg;
        oled_CFG_REG     = oled_backup.cfg;
        oled_ADDR_REG    = oled_backup.addr;
        oled_CLKDIV1_REG = oled_backup.clkDiv1;
        oled_CLKDIV2_REG = oled_backup.clkDiv2;
    }

#if (oled_WAKEUP_ENABLED)
    oled_DisableInt();
    (void) CyIntSetVector(oled_ISR_NUMBER, &oled_ISR);
    if (0u != oled_wakeupSource)
    {
        /* Generate interrupt to process incoming transaction */
        oled_SetPendingInt();
    }
    oled_EnableInt();
#endif /* (oled_WAKEUP_ENABLED) */

#else
    oled_CFG_REG = oled_backup.control;
#endif /* (oled_FF_IMPLEMENTED) */

#if (oled_TIMEOUT_ENABLED)
    oled_TimeoutRestoreConfig();
#endif /* (oled_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: oled_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component for active mode
*  operation (when device exits sleep mode). The Enable wakeup from Sleep Mode
*  selection influences this function implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              by calling I2C_RestoreConfig(). If the component was enabled
*              before the I2C_Sleep() function was called, I2C_Wakeup()
*              re-enables it.
*   Checked:   Enables  master functionality if it was enabled before sleep,
*              and disables the backup regulator of the I2C hardware.
*              The incoming transaction continues as soon as the regular
*              I2C interrupt handler is set up (global interrupts has to be
*              enabled to service I2C component interrupt).
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before oled_SaveConfig() or
*  oled_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void oled_Wakeup(void) 
{
    oled_RestoreConfig();

    /* Restore component enable state */
    if (0u != oled_backup.enableState)
    {
        oled_Enable();
        oled_EnableInt();
    }
    else
    {
    #if (oled_TIMEOUT_ENABLED)
        oled_TimeoutEnable();
    #endif /* (oled_TIMEOUT_ENABLED) */
    }
}


/* [] END OF FILE */
