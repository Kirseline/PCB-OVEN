/*******************************************************************************
* File Name: oled_MASTER.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component master mode.
*
*******************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "oled_PVT.h"

#if(oled_MODE_MASTER_ENABLED)

/**********************************
*      System variables
**********************************/

volatile uint8 oled_mstrStatus;     /* Master Status byte  */
volatile uint8 oled_mstrControl;    /* Master Control byte */

/* Transmit buffer variables */
volatile uint8 * oled_mstrRdBufPtr;     /* Pointer to Master Read buffer */
volatile uint8   oled_mstrRdBufSize;    /* Master Read buffer size       */
volatile uint8   oled_mstrRdBufIndex;   /* Master Read buffer Index      */

/* Receive buffer variables */
volatile uint8 * oled_mstrWrBufPtr;     /* Pointer to Master Write buffer */
volatile uint8   oled_mstrWrBufSize;    /* Master Write buffer size       */
volatile uint8   oled_mstrWrBufIndex;   /* Master Write buffer Index      */


/*******************************************************************************
* Function Name: oled_MasterWriteBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer of data to be sent.
*  cnt:       Size of buffer to send.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after a start or restart condition is
*  generated.
*
* Global variables:
*  oled_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  oled_state       - The global variable used to store a current
*                                 state of the software FSM.
*  oled_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without Stop
*                                 generation.
*  oled_mstrWrBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  oled_mstrWrBufIndex - The global variable used to store current
*                                    index within the master write buffer.
*  oled_mstrWrBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No
*
*******************************************************************************/
uint8 oled_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode)
      
{
    uint8 errStatus;

    errStatus = oled_MSTR_NOT_READY;

    if(NULL != wrData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(oled_SM_IDLE == oled_state)
        {
            /* Master is ready for transaction: check if bus is free */
            if(oled_CHECK_BUS_FREE(oled_MCSR_REG))
            {
                errStatus = oled_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = oled_MSTR_BUS_BUSY;
            }
        }
        else if(oled_SM_MSTR_HALT == oled_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = oled_MSTR_NO_ERROR;

            oled_ClearPendingInt();
            oled_mstrStatus &= (uint8) ~oled_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = oled_MSTR_NOT_READY was send before */
        }

        if(oled_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            oled_state = oled_SM_MSTR_WR_ADDR;

            /* Prepare write buffer */
            oled_mstrWrBufIndex = 0u;
            oled_mstrWrBufSize  = cnt;
            oled_mstrWrBufPtr   = (volatile uint8 *) wrData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            oled_mstrControl = mode;

            /* Clear write status history */
            oled_mstrStatus &= (uint8) ~oled_MSTAT_WR_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            oled_DATA_REG = (uint8) (slaveAddress << oled_SLAVE_ADDR_SHIFT);

            if(oled_CHECK_RESTART(mode))
            {
                oled_GENERATE_RESTART;
            }
            else
            {
                oled_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            oled_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: oled_MasterReadBuf
********************************************************************************
*
* Summary:
*  Automatically writes an entire buffer of data to a slave device. Once the
*  data transfer is initiated by this function, further data transfer is handled
*  by the included ISR in byte by byte mode.
*
* Parameters:
*  slaveAddr: 7-bit slave address.
*  xferData:  Pointer to buffer where to put data from slave.
*  cnt:       Size of buffer to read.
*  mode:      Transfer mode defines: start or restart condition generation at
*             begin of the transfer and complete the transfer or halt before
*             generating a stop.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  The included ISR will start a transfer after start or restart condition is
*  generated.
*
* Global variables:
*  oled_mstrStatus  - The global variable used to store a current
*                                 status of the I2C Master.
*  oled_state       - The global variable used to store a current
*                                 state of the software FSM.
*  oled_mstrControl - The global variable used to control the master
*                                 end of a transaction with or without
*                                 Stop generation.
*  oled_mstrRdBufPtr - The global variable used to store a pointer
*                                  to the master write buffer.
*  oled_mstrRdBufIndex - The global variable  used to store a
*                                    current index within the master
*                                    write buffer.
*  oled_mstrRdBufSize - The global variable used to store a master
*                                   write buffer size.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 oled_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode)
      
{
    uint8 errStatus;

    errStatus = oled_MSTR_NOT_READY;

    if(NULL != rdData)
    {
        /* Check I2C state to allow transfer: valid states are IDLE or HALT */
        if(oled_SM_IDLE == oled_state)
        {
            /* Master is ready to transaction: check if bus is free */
            if(oled_CHECK_BUS_FREE(oled_MCSR_REG))
            {
                errStatus = oled_MSTR_NO_ERROR;
            }
            else
            {
                errStatus = oled_MSTR_BUS_BUSY;
            }
        }
        else if(oled_SM_MSTR_HALT == oled_state)
        {
            /* Master is ready and waiting for ReStart */
            errStatus = oled_MSTR_NO_ERROR;

            oled_ClearPendingInt();
            oled_mstrStatus &= (uint8) ~oled_MSTAT_XFER_HALT;
        }
        else
        {
            /* errStatus = oled_MSTR_NOT_READY was set before */
        }

        if(oled_MSTR_NO_ERROR == errStatus)
        {
            /* Set state to start write transaction */
            oled_state = oled_SM_MSTR_RD_ADDR;

            /* Prepare read buffer */
            oled_mstrRdBufIndex  = 0u;
            oled_mstrRdBufSize   = cnt;
            oled_mstrRdBufPtr    = (volatile uint8 *) rdData;

            /* Set end of transaction flag: Stop or Halt (following ReStart) */
            oled_mstrControl = mode;

            /* Clear read status history */
            oled_mstrStatus &= (uint8) ~oled_MSTAT_RD_CMPLT;

            /* Hardware actions: write address and generate Start or ReStart */
            oled_DATA_REG = ((uint8) (slaveAddress << oled_SLAVE_ADDR_SHIFT) |
                                                  oled_READ_FLAG);

            if(oled_CHECK_RESTART(mode))
            {
                oled_GENERATE_RESTART;
            }
            else
            {
                oled_GENERATE_START;
            }

            /* Enable interrupt to complete transfer */
            oled_EnableInt();
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: oled_MasterSendStart
********************************************************************************
*
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  oled_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 oled_MasterSendStart(uint8 slaveAddress, uint8 R_nW)
      
{
    uint8 errStatus;

    errStatus = oled_MSTR_NOT_READY;

    /* If IDLE, check if bus is free */
    if(oled_SM_IDLE == oled_state)
    {
        /* If bus is free, generate Start condition */
        if(oled_CHECK_BUS_FREE(oled_MCSR_REG))
        {
            /* Disable interrupt for manual master operation */
            oled_DisableInt();

            /* Set address and read/write flag */
            slaveAddress = (uint8) (slaveAddress << oled_SLAVE_ADDR_SHIFT);
            if(0u != R_nW)
            {
                slaveAddress |= oled_READ_FLAG;
                oled_state = oled_SM_MSTR_RD_ADDR;
            }
            else
            {
                oled_state = oled_SM_MSTR_WR_ADDR;
            }

            /* Hardware actions: write address and generate Start */
            oled_DATA_REG = slaveAddress;
            oled_GENERATE_START_MANUAL;

            /* Wait until address is transferred */
            while(oled_WAIT_BYTE_COMPLETE(oled_CSR_REG))
            {
            }

        #if(oled_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(oled_CHECK_START_GEN(oled_MCSR_REG))
            {
                oled_CLEAR_START_GEN;

                /* Start condition was not generated: reset FSM to IDLE */
                oled_state = oled_SM_IDLE;
                errStatus = oled_MSTR_ERR_ABORT_START_GEN;
            }
            else
        #endif /* (oled_MODE_MULTI_MASTER_SLAVE_ENABLED) */

        #if(oled_MODE_MULTI_MASTER_ENABLED)
            if(oled_CHECK_LOST_ARB(oled_CSR_REG))
            {
                oled_BUS_RELEASE_MANUAL;

                /* Master lost arbitrage: reset FSM to IDLE */
                oled_state = oled_SM_IDLE;
                errStatus = oled_MSTR_ERR_ARB_LOST;
            }
            else
        #endif /* (oled_MODE_MULTI_MASTER_ENABLED) */

            if(oled_CHECK_ADDR_NAK(oled_CSR_REG))
            {
                /* Address has been NACKed: reset FSM to IDLE */
                oled_state = oled_SM_IDLE;
                errStatus = oled_MSTR_ERR_LB_NAK;
            }
            else
            {
                /* Start was sent without errors */
                errStatus = oled_MSTR_NO_ERROR;
            }
        }
        else
        {
            errStatus = oled_MSTR_BUS_BUSY;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: oled_MasterSendRestart
********************************************************************************
*
* Summary:
*  Generates ReStart condition and sends slave address with read/write bit.
*
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  oled_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 oled_MasterSendRestart(uint8 slaveAddress, uint8 R_nW)
      
{
    uint8 errStatus;

    errStatus = oled_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(oled_CHECK_MASTER_MODE(oled_MCSR_REG))
    {
        /* Set address and read/write flag */
        slaveAddress = (uint8) (slaveAddress << oled_SLAVE_ADDR_SHIFT);
        if(0u != R_nW)
        {
            slaveAddress |= oled_READ_FLAG;
            oled_state = oled_SM_MSTR_RD_ADDR;
        }
        else
        {
            oled_state = oled_SM_MSTR_WR_ADDR;
        }

        /* Hardware actions: write address and generate ReStart */
        oled_DATA_REG = slaveAddress;
        oled_GENERATE_RESTART_MANUAL;

        /* Wait until address has been transferred */
        while(oled_WAIT_BYTE_COMPLETE(oled_CSR_REG))
        {
        }

    #if(oled_MODE_MULTI_MASTER_ENABLED)
        if(oled_CHECK_LOST_ARB(oled_CSR_REG))
        {
            oled_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            oled_state = oled_SM_IDLE;
            errStatus = oled_MSTR_ERR_ARB_LOST;
        }
        else
    #endif /* (oled_MODE_MULTI_MASTER_ENABLED) */

        if(oled_CHECK_ADDR_NAK(oled_CSR_REG))
        {
            /* Address has been NACKed: reset FSM to IDLE */
            oled_state = oled_SM_IDLE;
            errStatus = oled_MSTR_ERR_LB_NAK;
        }
        else
        {
            /* ReStart was sent without errors */
            errStatus = oled_MSTR_NO_ERROR;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: oled_MasterSendStop
********************************************************************************
*
* Summary:
*  Generates I2C Stop condition on bus. Function do nothing if Start or Restart
*  condition was failed before call this function.
*
* Parameters:
*  None.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  Stop generation is required to complete the transaction.
*  This function does not wait until a Stop condition is generated.
*
* Global variables:
*  oled_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 oled_MasterSendStop(void) 
{
    uint8 errStatus;

    errStatus = oled_MSTR_NOT_READY;

    /* Check if master is active on bus */
    if(oled_CHECK_MASTER_MODE(oled_MCSR_REG))
    {
        oled_GENERATE_STOP_MANUAL;
        oled_state = oled_SM_IDLE;

        /* Wait until stop has been generated */
        while(oled_WAIT_STOP_COMPLETE(oled_CSR_REG))
        {
        }

        errStatus = oled_MSTR_NO_ERROR;

    #if(oled_MODE_MULTI_MASTER_ENABLED)
        if(oled_CHECK_LOST_ARB(oled_CSR_REG))
        {
            oled_BUS_RELEASE_MANUAL;

            /* NACK was generated by instead Stop */
            errStatus = oled_MSTR_ERR_ARB_LOST;
        }
    #endif /* (oled_MODE_MULTI_MASTER_ENABLED) */
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: oled_MasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave. A valid Start or ReStart condition must be
*  generated before this call this function. Function do nothing if Start or
*  Restart condition was failed before call this function.
*
* Parameters:
*  data:  The data byte to send to the slave.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  oled_state - The global variable used to store a current state of
*                           the software FSM.
*
*******************************************************************************/
uint8 oled_MasterWriteByte(uint8 theByte) 
{
    uint8 errStatus;

    errStatus = oled_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(oled_CHECK_MASTER_MODE(oled_MCSR_REG))
    {
        oled_DATA_REG = theByte;   /* Write DATA register */
        oled_TRANSMIT_DATA_MANUAL; /* Set transmit mode   */
        oled_state = oled_SM_MSTR_WR_DATA;

        /* Wait until data byte has been transmitted */
        while(oled_WAIT_BYTE_COMPLETE(oled_CSR_REG))
        {
        }

    #if(oled_MODE_MULTI_MASTER_ENABLED)
        if(oled_CHECK_LOST_ARB(oled_CSR_REG))
        {
            oled_BUS_RELEASE_MANUAL;

            /* Master lost arbitrage: reset FSM to IDLE */
            oled_state = oled_SM_IDLE;
            errStatus = oled_MSTR_ERR_ARB_LOST;
        }
        /* Check LRB bit */
        else
    #endif /* (oled_MODE_MULTI_MASTER_ENABLED) */

        if(oled_CHECK_DATA_ACK(oled_CSR_REG))
        {
            oled_state = oled_SM_MSTR_HALT;
            errStatus = oled_MSTR_NO_ERROR;
        }
        else
        {
            oled_state = oled_SM_MSTR_HALT;
            errStatus = oled_MSTR_ERR_LB_NAK;
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: oled_MasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACK or NACK the transfer. A valid Start or
*  ReStart condition must be generated before this call this function. Function
*  do nothing if Start or Restart condition was failed before call this
*  function.
*
* Parameters:
*  acknNack:  Zero, response with NACK, if non-zero response with ACK.
*
* Return:
*  Byte read from slave.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  oled_state - The global variable used to store a current
*                           state of the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 oled_MasterReadByte(uint8 acknNak) 
{
    uint8 theByte;

    theByte = 0u;

    /* Check if START condition was generated */
    if(oled_CHECK_MASTER_MODE(oled_MCSR_REG))
    {
        /* When address phase needs to release bus and receive byte,
        * then decide ACK or NACK
        */
        if(oled_SM_MSTR_RD_ADDR == oled_state)
        {
            oled_READY_TO_READ_MANUAL;
            oled_state = oled_SM_MSTR_RD_DATA;
        }

        /* Wait until data byte has been received */
        while(oled_WAIT_BYTE_COMPLETE(oled_CSR_REG))
        {
        }

        theByte = oled_DATA_REG;

        /* Command ACK to receive next byte and continue transfer.
        *  Do nothing for NACK. The NACK will be generated by
        *  Stop or ReStart routine.
        */
        if(acknNak != 0u) /* Generate ACK */
        {
            oled_ACK_AND_RECEIVE_MANUAL;
        }
        else              /* Do nothing for the follwong NACK */
        {
            oled_state = oled_SM_MSTR_HALT;
        }
    }

    return(theByte);
}


/*******************************************************************************
* Function Name: oled_MasterStatus
********************************************************************************
*
* Summary:
*  Returns the master's communication status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  oled_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
*******************************************************************************/
uint8 oled_MasterStatus(void) 
{
    uint8 status;

    oled_DisableInt(); /* Lock from interrupt */

    /* Read master status */
    status = oled_mstrStatus;

    if (oled_CHECK_SM_MASTER)
    {
        /* Set transfer in progress flag in status */
        status |= oled_MSTAT_XFER_INP;
    }

    oled_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: oled_MasterClearStatus
********************************************************************************
*
* Summary:
*  Clears all status flags and returns the master status.
*
* Parameters:
*  None.
*
* Return:
*  Current status of I2C master.
*
* Global variables:
*  oled_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 oled_MasterClearStatus(void) 
{
    uint8 status;

    oled_DisableInt(); /* Lock from interrupt */

    /* Read and clear master status */
    status = oled_mstrStatus;
    oled_mstrStatus = oled_MSTAT_CLEAR;

    oled_EnableInt(); /* Release lock */

    return (status);
}


/*******************************************************************************
* Function Name: oled_MasterGetReadBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterReadBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  oled_mstrRdBufIndex - The global variable stores current index
*                                    within the master read buffer.
*
*******************************************************************************/
uint8 oled_MasterGetReadBufSize(void) 
{
    return (oled_mstrRdBufIndex);
}


/*******************************************************************************
* Function Name: oled_MasterGetWriteBufSize
********************************************************************************
*
* Summary:
*  Returns the amount of bytes that has been transferred with an
*  I2C_MasterWriteBuf command.
*
* Parameters:
*  None.
*
* Return:
*  Byte count of transfer. If the transfer is not yet complete, it will return
*  the byte count transferred so far.
*
* Global variables:
*  oled_mstrWrBufIndex -  The global variable used to stores current
*                                     index within master write buffer.
*
*******************************************************************************/
uint8 oled_MasterGetWriteBufSize(void) 
{
    return (oled_mstrWrBufIndex);
}


/*******************************************************************************
* Function Name: oled_MasterClearReadBuf
********************************************************************************
*
* Summary:
*  Resets the read buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  oled_mstrRdBufIndex - The global variable used to stores current
*                                    index within master read buffer.
*  oled_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void oled_MasterClearReadBuf(void) 
{
    oled_DisableInt(); /* Lock from interrupt */

    oled_mstrRdBufIndex = 0u;
    oled_mstrStatus    &= (uint8) ~oled_MSTAT_RD_CMPLT;

    oled_EnableInt(); /* Release lock */
}


/*******************************************************************************
* Function Name: oled_MasterClearWriteBuf
********************************************************************************
*
* Summary:
*  Resets the write buffer pointer back to the first byte in the buffer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  oled_mstrRdBufIndex - The global variable used to stote current
*                                    index within master read buffer.
*  oled_mstrStatus - The global variable used to store a current
*                                status of the I2C Master.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void oled_MasterClearWriteBuf(void) 
{
    oled_DisableInt(); /* Lock from interrupt */

    oled_mstrWrBufIndex = 0u;
    oled_mstrStatus    &= (uint8) ~oled_MSTAT_WR_CMPLT;

    oled_EnableInt(); /* Release lock */
}

#endif /* (oled_MODE_MASTER_ENABLED) */


/* [] END OF FILE */
