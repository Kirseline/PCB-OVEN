/*******************************************************************************
* File Name: oled_INT.c
* Version 3.50
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "oled_PVT.h"
#include "cyapicallbacks.h"


/*******************************************************************************
*  Place your includes, defines and code here.
********************************************************************************/
/* `#START oled_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: oled_ISR
********************************************************************************
*
* Summary:
*  The handler for the I2C interrupt. The slave and master operations are
*  handled here.
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
CY_ISR(oled_ISR)
{
#if (oled_MODE_SLAVE_ENABLED)
   uint8  tmp8;
#endif  /* (oled_MODE_SLAVE_ENABLED) */

    uint8  tmpCsr;
    
#ifdef oled_ISR_ENTRY_CALLBACK
    oled_ISR_EntryCallback();
#endif /* oled_ISR_ENTRY_CALLBACK */
    

#if(oled_TIMEOUT_FF_ENABLED)
    if(0u != oled_TimeoutGetStatus())
    {
        oled_TimeoutReset();
        oled_state = oled_SM_EXIT_IDLE;
        /* oled_CSR_REG should be cleared after reset */
    }
#endif /* (oled_TIMEOUT_FF_ENABLED) */


    tmpCsr = oled_CSR_REG;      /* Make copy as interrupts clear */

#if(oled_MODE_MULTI_MASTER_SLAVE_ENABLED)
    if(oled_CHECK_START_GEN(oled_MCSR_REG))
    {
        oled_CLEAR_START_GEN;

        /* Set transfer complete and error flags */
        oled_mstrStatus |= (oled_MSTAT_ERR_XFER |
                                        oled_GET_MSTAT_CMPLT);

        /* Slave was addressed */
        oled_state = oled_SM_SLAVE;
    }
#endif /* (oled_MODE_MULTI_MASTER_SLAVE_ENABLED) */


#if(oled_MODE_MULTI_MASTER_ENABLED)
    if(oled_CHECK_LOST_ARB(tmpCsr))
    {
        /* Set errors */
        oled_mstrStatus |= (oled_MSTAT_ERR_XFER     |
                                        oled_MSTAT_ERR_ARB_LOST |
                                        oled_GET_MSTAT_CMPLT);

        oled_DISABLE_INT_ON_STOP; /* Interrupt on Stop is enabled by write */

        #if(oled_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(oled_CHECK_ADDRESS_STS(tmpCsr))
            {
                /* Slave was addressed */
                oled_state = oled_SM_SLAVE;
            }
            else
            {
                oled_BUS_RELEASE;

                oled_state = oled_SM_EXIT_IDLE;
            }
        #else
            oled_BUS_RELEASE;

            oled_state = oled_SM_EXIT_IDLE;

        #endif /* (oled_MODE_MULTI_MASTER_SLAVE_ENABLED) */
    }
#endif /* (oled_MODE_MULTI_MASTER_ENABLED) */

    /* Check for master operation mode */
    if(oled_CHECK_SM_MASTER)
    {
    #if(oled_MODE_MASTER_ENABLED)
        if(oled_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            switch (oled_state)
            {
            case oled_SM_MSTR_WR_ADDR:  /* After address is sent, write data */
            case oled_SM_MSTR_RD_ADDR:  /* After address is sent, read data */

                tmpCsr &= ((uint8) ~oled_CSR_STOP_STATUS); /* Clear Stop bit history on address phase */

                if(oled_CHECK_ADDR_ACK(tmpCsr))
                {
                    /* Setup for transmit or receive of data */
                    if(oled_state == oled_SM_MSTR_WR_ADDR)   /* TRANSMIT data */
                    {
                        /* Check if at least one byte to transfer */
                        if(oled_mstrWrBufSize > 0u)
                        {
                            /* Load the 1st data byte */
                            oled_DATA_REG = oled_mstrWrBufPtr[0u];
                            oled_TRANSMIT_DATA;
                            oled_mstrWrBufIndex = 1u;   /* Set index to 2nd element */

                            /* Set transmit state until done */
                            oled_state = oled_SM_MSTR_WR_DATA;
                        }
                        /* End of buffer: complete writing */
                        else if(oled_CHECK_NO_STOP(oled_mstrControl))
                        {
                            /* Set write complete and master halted */
                            oled_mstrStatus |= (oled_MSTAT_XFER_HALT |
                                                            oled_MSTAT_WR_CMPLT);

                            oled_state = oled_SM_MSTR_HALT; /* Expect ReStart */
                            oled_DisableInt();
                        }
                        else
                        {
                            oled_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                            oled_GENERATE_STOP;
                        }
                    }
                    else  /* Master receive data */
                    {
                        oled_READY_TO_READ; /* Release bus to read data */

                        oled_state  = oled_SM_MSTR_RD_DATA;
                    }
                }
                /* Address is NACKed */
                else if(oled_CHECK_ADDR_NAK(tmpCsr))
                {
                    /* Set Address NAK error */
                    oled_mstrStatus |= (oled_MSTAT_ERR_XFER |
                                                    oled_MSTAT_ERR_ADDR_NAK);

                    if(oled_CHECK_NO_STOP(oled_mstrControl))
                    {
                        oled_mstrStatus |= (oled_MSTAT_XFER_HALT |
                                                        oled_GET_MSTAT_CMPLT);

                        oled_state = oled_SM_MSTR_HALT; /* Expect RESTART */
                        oled_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        oled_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                        oled_GENERATE_STOP;
                    }
                }
                else
                {
                    /* Address phase is not set for some reason: error */
                    #if(oled_TIMEOUT_ENABLED)
                        /* Exit interrupt to take chance for timeout timer to handle this case */
                        oled_DisableInt();
                        oled_ClearPendingInt();
                    #else
                        /* Block execution flow: unexpected condition */
                        CYASSERT(0u != 0u);
                    #endif /* (oled_TIMEOUT_ENABLED) */
                }
                break;

            case oled_SM_MSTR_WR_DATA:

                if(oled_CHECK_DATA_ACK(tmpCsr))
                {
                    /* Check if end of buffer */
                    if(oled_mstrWrBufIndex  < oled_mstrWrBufSize)
                    {
                        oled_DATA_REG =
                                                 oled_mstrWrBufPtr[oled_mstrWrBufIndex];
                        oled_TRANSMIT_DATA;
                        oled_mstrWrBufIndex++;
                    }
                    /* End of buffer: complete writing */
                    else if(oled_CHECK_NO_STOP(oled_mstrControl))
                    {
                        /* Set write complete and master halted */
                        oled_mstrStatus |= (oled_MSTAT_XFER_HALT |
                                                        oled_MSTAT_WR_CMPLT);

                        oled_state = oled_SM_MSTR_HALT;    /* Expect restart */
                        oled_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        oled_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                        oled_GENERATE_STOP;
                    }
                }
                /* Last byte NAKed: end writing */
                else if(oled_CHECK_NO_STOP(oled_mstrControl))
                {
                    /* Set write complete, short transfer and master halted */
                    oled_mstrStatus |= (oled_MSTAT_ERR_XFER       |
                                                    oled_MSTAT_ERR_SHORT_XFER |
                                                    oled_MSTAT_XFER_HALT      |
                                                    oled_MSTAT_WR_CMPLT);

                    oled_state = oled_SM_MSTR_HALT;    /* Expect ReStart */
                    oled_DisableInt();
                }
                else  /* Do normal Stop */
                {
                    oled_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                    oled_GENERATE_STOP;

                    /* Set short transfer and error flag */
                    oled_mstrStatus |= (oled_MSTAT_ERR_SHORT_XFER |
                                                    oled_MSTAT_ERR_XFER);
                }

                break;

            case oled_SM_MSTR_RD_DATA:

                oled_mstrRdBufPtr[oled_mstrRdBufIndex] = oled_DATA_REG;
                oled_mstrRdBufIndex++;

                /* Check if end of buffer */
                if(oled_mstrRdBufIndex < oled_mstrRdBufSize)
                {
                    oled_ACK_AND_RECEIVE;       /* ACK and receive byte */
                }
                /* End of buffer: complete reading */
                else if(oled_CHECK_NO_STOP(oled_mstrControl))
                {
                    /* Set read complete and master halted */
                    oled_mstrStatus |= (oled_MSTAT_XFER_HALT |
                                                    oled_MSTAT_RD_CMPLT);

                    oled_state = oled_SM_MSTR_HALT;    /* Expect ReStart */
                    oled_DisableInt();
                }
                else
                {
                    oled_ENABLE_INT_ON_STOP;
                    oled_NAK_AND_RECEIVE;       /* NACK and TRY to generate Stop */
                }
                break;

            default: /* This is an invalid state and should not occur */

            #if(oled_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handles this case */
                oled_DisableInt();
                oled_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (oled_TIMEOUT_ENABLED) */

                break;
            }
        }

        /* Catches Stop: end of transaction */
        if(oled_CHECK_STOP_STS(tmpCsr))
        {
            oled_mstrStatus |= oled_GET_MSTAT_CMPLT;

            oled_DISABLE_INT_ON_STOP;
            oled_state = oled_SM_IDLE;
        }
    #endif /* (oled_MODE_MASTER_ENABLED) */
    }
    else if(oled_CHECK_SM_SLAVE)
    {
    #if(oled_MODE_SLAVE_ENABLED)

        if((oled_CHECK_STOP_STS(tmpCsr)) || /* Stop || Restart */
           (oled_CHECK_BYTE_COMPLETE(tmpCsr) && oled_CHECK_ADDRESS_STS(tmpCsr)))
        {
            /* Catch end of master write transaction: use interrupt on Stop */
            /* The Stop bit history on address phase does not have correct state */
            if(oled_SM_SL_WR_DATA == oled_state)
            {
                oled_DISABLE_INT_ON_STOP;

                oled_slStatus &= ((uint8) ~oled_SSTAT_WR_BUSY);
                oled_slStatus |= ((uint8)  oled_SSTAT_WR_CMPLT);

                oled_state = oled_SM_IDLE;
            }
        }

        if(oled_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            /* The address only issued after Start or ReStart: so check the address
               to catch these events:
                FF : sets an address phase with a byte_complete interrupt trigger.
                UDB: sets an address phase immediately after Start or ReStart. */
            if(oled_CHECK_ADDRESS_STS(tmpCsr))
            {
            /* Check for software address detection */
            #if(oled_SW_ADRR_DECODE)
                tmp8 = oled_GET_SLAVE_ADDR(oled_DATA_REG);

                if(tmp8 == oled_slAddress)   /* Check for address match */
                {
                    if(0u != (oled_DATA_REG & oled_READ_FLAG))
                    {
                        /* Place code to prepare read buffer here                  */
                        /* `#START oled_SW_PREPARE_READ_BUF_interrupt` */

                        /* `#END` */

                    #ifdef oled_SW_PREPARE_READ_BUF_CALLBACK
                        oled_SwPrepareReadBuf_Callback();
                    #endif /* oled_SW_PREPARE_READ_BUF_CALLBACK */
                        
                        /* Prepare next operation to read, get data and place in data register */
                        if(oled_slRdBufIndex < oled_slRdBufSize)
                        {
                            /* Load first data byte from array */
                            oled_DATA_REG = oled_slRdBufPtr[oled_slRdBufIndex];
                            oled_ACK_AND_TRANSMIT;
                            oled_slRdBufIndex++;

                            oled_slStatus |= oled_SSTAT_RD_BUSY;
                        }
                        else    /* Overflow: provide 0xFF on bus */
                        {
                            oled_DATA_REG = oled_OVERFLOW_RETURN;
                            oled_ACK_AND_TRANSMIT;

                            oled_slStatus  |= (oled_SSTAT_RD_BUSY |
                                                           oled_SSTAT_RD_ERR_OVFL);
                        }

                        oled_state = oled_SM_SL_RD_DATA;
                    }
                    else  /* Write transaction: receive 1st byte */
                    {
                        oled_ACK_AND_RECEIVE;
                        oled_state = oled_SM_SL_WR_DATA;

                        oled_slStatus |= oled_SSTAT_WR_BUSY;
                        oled_ENABLE_INT_ON_STOP;
                    }
                }
                else
                {
                    /*     Place code to compare for additional address here    */
                    /* `#START oled_SW_ADDR_COMPARE_interruptStart` */

                    /* `#END` */

                #ifdef oled_SW_ADDR_COMPARE_ENTRY_CALLBACK
                    oled_SwAddrCompare_EntryCallback();
                #endif /* oled_SW_ADDR_COMPARE_ENTRY_CALLBACK */
                    
                    oled_NAK_AND_RECEIVE;   /* NACK address */

                    /* Place code to end of condition for NACK generation here */
                    /* `#START oled_SW_ADDR_COMPARE_interruptEnd`  */

                    /* `#END` */

                #ifdef oled_SW_ADDR_COMPARE_EXIT_CALLBACK
                    oled_SwAddrCompare_ExitCallback();
                #endif /* oled_SW_ADDR_COMPARE_EXIT_CALLBACK */
                }

            #else /* (oled_HW_ADRR_DECODE) */

                if(0u != (oled_DATA_REG & oled_READ_FLAG))
                {
                    /* Place code to prepare read buffer here                  */
                    /* `#START oled_HW_PREPARE_READ_BUF_interrupt` */

                    /* `#END` */
                    
                #ifdef oled_HW_PREPARE_READ_BUF_CALLBACK
                    oled_HwPrepareReadBuf_Callback();
                #endif /* oled_HW_PREPARE_READ_BUF_CALLBACK */

                    /* Prepare next operation to read, get data and place in data register */
                    if(oled_slRdBufIndex < oled_slRdBufSize)
                    {
                        /* Load first data byte from array */
                        oled_DATA_REG = oled_slRdBufPtr[oled_slRdBufIndex];
                        oled_ACK_AND_TRANSMIT;
                        oled_slRdBufIndex++;

                        oled_slStatus |= oled_SSTAT_RD_BUSY;
                    }
                    else    /* Overflow: provide 0xFF on bus */
                    {
                        oled_DATA_REG = oled_OVERFLOW_RETURN;
                        oled_ACK_AND_TRANSMIT;

                        oled_slStatus  |= (oled_SSTAT_RD_BUSY |
                                                       oled_SSTAT_RD_ERR_OVFL);
                    }

                    oled_state = oled_SM_SL_RD_DATA;
                }
                else  /* Write transaction: receive 1st byte */
                {
                    oled_ACK_AND_RECEIVE;
                    oled_state = oled_SM_SL_WR_DATA;

                    oled_slStatus |= oled_SSTAT_WR_BUSY;
                    oled_ENABLE_INT_ON_STOP;
                }

            #endif /* (oled_SW_ADRR_DECODE) */
            }
            /* Data states */
            /* Data master writes into slave */
            else if(oled_state == oled_SM_SL_WR_DATA)
            {
                if(oled_slWrBufIndex < oled_slWrBufSize)
                {
                    tmp8 = oled_DATA_REG;
                    oled_ACK_AND_RECEIVE;
                    oled_slWrBufPtr[oled_slWrBufIndex] = tmp8;
                    oled_slWrBufIndex++;
                }
                else  /* of array: complete write, send NACK */
                {
                    oled_NAK_AND_RECEIVE;

                    oled_slStatus |= oled_SSTAT_WR_ERR_OVFL;
                }
            }
            /* Data master reads from slave */
            else if(oled_state == oled_SM_SL_RD_DATA)
            {
                if(oled_CHECK_DATA_ACK(tmpCsr))
                {
                    if(oled_slRdBufIndex < oled_slRdBufSize)
                    {
                         /* Get data from array */
                        oled_DATA_REG = oled_slRdBufPtr[oled_slRdBufIndex];
                        oled_TRANSMIT_DATA;
                        oled_slRdBufIndex++;
                    }
                    else   /* Overflow: provide 0xFF on bus */
                    {
                        oled_DATA_REG = oled_OVERFLOW_RETURN;
                        oled_TRANSMIT_DATA;

                        oled_slStatus |= oled_SSTAT_RD_ERR_OVFL;
                    }
                }
                else  /* Last byte was NACKed: read complete */
                {
                    /* Only NACK appears on bus */
                    oled_DATA_REG = oled_OVERFLOW_RETURN;
                    oled_NAK_AND_TRANSMIT;

                    oled_slStatus &= ((uint8) ~oled_SSTAT_RD_BUSY);
                    oled_slStatus |= ((uint8)  oled_SSTAT_RD_CMPLT);

                    oled_state = oled_SM_IDLE;
                }
            }
            else
            {
            #if(oled_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handle this case */
                oled_DisableInt();
                oled_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (oled_TIMEOUT_ENABLED) */
            }
        }
    #endif /* (oled_MODE_SLAVE_ENABLED) */
    }
    else
    {
        /* The FSM skips master and slave processing: return to IDLE */
        oled_state = oled_SM_IDLE;
    }

#ifdef oled_ISR_EXIT_CALLBACK
    oled_ISR_ExitCallback();
#endif /* oled_ISR_EXIT_CALLBACK */    
}


#if ((oled_FF_IMPLEMENTED) && (oled_WAKEUP_ENABLED))
    /*******************************************************************************
    * Function Name: oled_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(oled_WAKEUP_ISR)
    {
    #ifdef oled_WAKEUP_ISR_ENTRY_CALLBACK
        oled_WAKEUP_ISR_EntryCallback();
    #endif /* oled_WAKEUP_ISR_ENTRY_CALLBACK */
         
        /* Set flag to notify that matched address is received */
        oled_wakeupSource = 1u;

        /* SCL is stretched until the I2C_Wake() is called */

    #ifdef oled_WAKEUP_ISR_EXIT_CALLBACK
        oled_WAKEUP_ISR_ExitCallback();
    #endif /* oled_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* ((oled_FF_IMPLEMENTED) && (oled_WAKEUP_ENABLED)) */


/* [] END OF FILE */
