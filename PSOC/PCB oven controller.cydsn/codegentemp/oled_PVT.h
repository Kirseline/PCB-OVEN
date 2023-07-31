/*******************************************************************************
* File Name: .h
* Version 3.50
*
* Description:
*  This file provides private constants and parameter values for the I2C
*  component.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_PVT_oled_H)
#define CY_I2C_PVT_oled_H

#include "oled.h"

#define oled_TIMEOUT_ENABLED_INC    (0u)
#if (0u != oled_TIMEOUT_ENABLED_INC)
    #include "oled_TMOUT.h"
#endif /* (0u != oled_TIMEOUT_ENABLED_INC) */


/**********************************
*   Variables with external linkage
**********************************/

extern oled_BACKUP_STRUCT oled_backup;

extern volatile uint8 oled_state;   /* Current state of I2C FSM */

/* Master variables */
#if (oled_MODE_MASTER_ENABLED)
    extern volatile uint8 oled_mstrStatus;   /* Master Status byte  */
    extern volatile uint8 oled_mstrControl;  /* Master Control byte */

    /* Transmit buffer variables */
    extern volatile uint8 * oled_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint8   oled_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint8   oled_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Receive buffer variables */
    extern volatile uint8 * oled_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint8   oled_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint8   oled_mstrWrBufIndex; /* Master Write buffer Index      */

#endif /* (oled_MODE_MASTER_ENABLED) */

/* Slave variables */
#if (oled_MODE_SLAVE_ENABLED)
    extern volatile uint8 oled_slStatus;         /* Slave Status  */

    /* Transmit buffer variables */
    extern volatile uint8 * oled_slRdBufPtr;     /* Pointer to Transmit buffer  */
    extern volatile uint8   oled_slRdBufSize;    /* Slave Transmit buffer size  */
    extern volatile uint8   oled_slRdBufIndex;   /* Slave Transmit buffer Index */

    /* Receive buffer variables */
    extern volatile uint8 * oled_slWrBufPtr;     /* Pointer to Receive buffer  */
    extern volatile uint8   oled_slWrBufSize;    /* Slave Receive buffer size  */
    extern volatile uint8   oled_slWrBufIndex;   /* Slave Receive buffer Index */

    #if (oled_SW_ADRR_DECODE)
        extern volatile uint8 oled_slAddress;     /* Software address variable */
    #endif   /* (oled_SW_ADRR_DECODE) */

#endif /* (oled_MODE_SLAVE_ENABLED) */

#if ((oled_FF_IMPLEMENTED) && (oled_WAKEUP_ENABLED))
    extern volatile uint8 oled_wakeupSource;
#endif /* ((oled_FF_IMPLEMENTED) && (oled_WAKEUP_ENABLED)) */


#endif /* CY_I2C_PVT_oled_H */


/* [] END OF FILE */
