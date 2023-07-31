/*******************************************************************************
* File Name: oled.h
* Version 3.50
*
* Description:
*  This file provides constants and parameter values for the I2C component.

*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_oled_H)
#define CY_I2C_oled_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"

/* Check if required defines such as CY_PSOC5LP are available in cy_boot */
#if !defined (CY_PSOC5LP)
    #error Component I2C_v3_50 requires cy_boot v3.10 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*   Conditional Compilation Parameters
****************************************/

#define oled_IMPLEMENTATION     (1u)
#define oled_MODE               (2u)
#define oled_ENABLE_WAKEUP      (0u)
#define oled_ADDR_DECODE        (1u)
#define oled_UDB_INTRN_CLOCK    (0u)


/* I2C implementation enum */
#define oled_UDB    (0x00u)
#define oled_FF     (0x01u)

#define oled_FF_IMPLEMENTED     (oled_FF  == oled_IMPLEMENTATION)
#define oled_UDB_IMPLEMENTED    (oled_UDB == oled_IMPLEMENTATION)

#define oled_UDB_INTRN_CLOCK_ENABLED    (oled_UDB_IMPLEMENTED && \
                                                     (0u != oled_UDB_INTRN_CLOCK))
/* I2C modes enum */
#define oled_MODE_SLAVE                 (0x01u)
#define oled_MODE_MASTER                (0x02u)
#define oled_MODE_MULTI_MASTER          (0x06u)
#define oled_MODE_MULTI_MASTER_SLAVE    (0x07u)
#define oled_MODE_MULTI_MASTER_MASK     (0x04u)

#define oled_MODE_SLAVE_ENABLED         (0u != (oled_MODE_SLAVE  & oled_MODE))
#define oled_MODE_MASTER_ENABLED        (0u != (oled_MODE_MASTER & oled_MODE))
#define oled_MODE_MULTI_MASTER_ENABLED  (0u != (oled_MODE_MULTI_MASTER_MASK & \
                                                            oled_MODE))
#define oled_MODE_MULTI_MASTER_SLAVE_ENABLED    (oled_MODE_MULTI_MASTER_SLAVE == \
                                                             oled_MODE)

/* Address detection enum */
#define oled_SW_DECODE      (0x00u)
#define oled_HW_DECODE      (0x01u)

#define oled_SW_ADRR_DECODE             (oled_SW_DECODE == oled_ADDR_DECODE)
#define oled_HW_ADRR_DECODE             (oled_HW_DECODE == oled_ADDR_DECODE)

/* Wakeup enabled */
#define oled_WAKEUP_ENABLED             (0u != oled_ENABLE_WAKEUP)

/* Adds bootloader APIs to component */
#define oled_BOOTLOADER_INTERFACE_ENABLED   (oled_MODE_SLAVE_ENABLED && \
                                                            ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_oled) || \
                                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface)))

/* Timeout functionality */
#define oled_TIMEOUT_ENABLE             (0u)
#define oled_TIMEOUT_SCL_TMOUT_ENABLE   (0u)
#define oled_TIMEOUT_SDA_TMOUT_ENABLE   (0u)
#define oled_TIMEOUT_PRESCALER_ENABLE   (0u)
#define oled_TIMEOUT_IMPLEMENTATION     (0u)

/* Convert to boolean */
#define oled_TIMEOUT_ENABLED            (0u != oled_TIMEOUT_ENABLE)
#define oled_TIMEOUT_SCL_TMOUT_ENABLED  (0u != oled_TIMEOUT_SCL_TMOUT_ENABLE)
#define oled_TIMEOUT_SDA_TMOUT_ENABLED  (0u != oled_TIMEOUT_SDA_TMOUT_ENABLE)
#define oled_TIMEOUT_PRESCALER_ENABLED  (0u != oled_TIMEOUT_PRESCALER_ENABLE)

/* Timeout implementation enum. */
#define oled_TIMEOUT_UDB    (0x00u)
#define oled_TIMEOUT_FF     (0x01u)

#define oled_TIMEOUT_FF_IMPLEMENTED     (oled_TIMEOUT_FF  == \
                                                        oled_TIMEOUT_IMPLEMENTATION)
#define oled_TIMEOUT_UDB_IMPLEMENTED    (oled_TIMEOUT_UDB == \
                                                        oled_TIMEOUT_IMPLEMENTATION)

#define oled_TIMEOUT_FF_ENABLED         (oled_TIMEOUT_ENABLED && \
                                                     oled_TIMEOUT_FF_IMPLEMENTED)

#define oled_TIMEOUT_UDB_ENABLED        (oled_TIMEOUT_ENABLED && \
                                                     oled_TIMEOUT_UDB_IMPLEMENTED)

#define oled_EXTERN_I2C_INTR_HANDLER    (0u)
#define oled_EXTERN_TMOUT_INTR_HANDLER  (0u)

#define oled_INTERN_I2C_INTR_HANDLER    (0u == oled_EXTERN_I2C_INTR_HANDLER)
#define oled_INTERN_TMOUT_INTR_HANDLER  (0u == oled_EXTERN_TMOUT_INTR_HANDLER)


/***************************************
*       Type defines
***************************************/

/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;

#if (oled_FF_IMPLEMENTED)
    uint8 xcfg;
    uint8 cfg;
    uint8 addr;
    uint8 clkDiv1;
    uint8 clkDiv2;
#else
    uint8 control;
#endif /* (oled_FF_IMPLEMENTED) */

#if (oled_TIMEOUT_ENABLED)
    uint16 tmoutCfg;
    uint8  tmoutIntr;
#endif /* (oled_TIMEOUT_ENABLED) */

} oled_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void oled_Init(void)                            ;
void oled_Enable(void)                          ;

void oled_Start(void)                           ;
void oled_Stop(void)                            ;

#define oled_EnableInt()        CyIntEnable      (oled_ISR_NUMBER)
#define oled_DisableInt()       CyIntDisable     (oled_ISR_NUMBER)
#define oled_ClearPendingInt()  CyIntClearPending(oled_ISR_NUMBER)
#define oled_SetPendingInt()    CyIntSetPending  (oled_ISR_NUMBER)

void oled_SaveConfig(void)                      ;
void oled_Sleep(void)                           ;
void oled_RestoreConfig(void)                   ;
void oled_Wakeup(void)                          ;

/* I2C Master functions prototypes */
#if (oled_MODE_MASTER_ENABLED)
    /* Read and Clear status functions */
    uint8 oled_MasterStatus(void)                ;
    uint8 oled_MasterClearStatus(void)           ;

    /* Interrupt based operation functions */
    uint8 oled_MasterWriteBuf(uint8 slaveAddress, uint8 * wrData, uint8 cnt, uint8 mode) \
                                                            ;
    uint8 oled_MasterReadBuf(uint8 slaveAddress, uint8 * rdData, uint8 cnt, uint8 mode) \
                                                            ;
    uint8 oled_MasterGetReadBufSize(void)       ;
    uint8 oled_MasterGetWriteBufSize(void)      ;
    void  oled_MasterClearReadBuf(void)         ;
    void  oled_MasterClearWriteBuf(void)        ;

    /* Manual operation functions */
    uint8 oled_MasterSendStart(uint8 slaveAddress, uint8 R_nW) \
                                                            ;
    uint8 oled_MasterSendRestart(uint8 slaveAddress, uint8 R_nW) \
                                                            ;
    uint8 oled_MasterSendStop(void)             ;
    uint8 oled_MasterWriteByte(uint8 theByte)   ;
    uint8 oled_MasterReadByte(uint8 acknNak)    ;

#endif /* (oled_MODE_MASTER_ENABLED) */

/* I2C Slave functions prototypes */
#if (oled_MODE_SLAVE_ENABLED)
    /* Read and Clear status functions */
    uint8 oled_SlaveStatus(void)                ;
    uint8 oled_SlaveClearReadStatus(void)       ;
    uint8 oled_SlaveClearWriteStatus(void)      ;

    void  oled_SlaveSetAddress(uint8 address)   ;

    /* Interrupt based operation functions */
    void  oled_SlaveInitReadBuf(uint8 * rdBuf, uint8 bufSize) \
                                                            ;
    void  oled_SlaveInitWriteBuf(uint8 * wrBuf, uint8 bufSize) \
                                                            ;
    uint8 oled_SlaveGetReadBufSize(void)        ;
    uint8 oled_SlaveGetWriteBufSize(void)       ;
    void  oled_SlaveClearReadBuf(void)          ;
    void  oled_SlaveClearWriteBuf(void)         ;

    /* Communication bootloader I2C Slave APIs */
    #if defined(CYDEV_BOOTLOADER_IO_COMP) && (oled_BOOTLOADER_INTERFACE_ENABLED)
        /* Physical layer functions */
        void     oled_CyBtldrCommStart(void) CYSMALL \
                                                            ;
        void     oled_CyBtldrCommStop(void)  CYSMALL \
                                                            ;
        void     oled_CyBtldrCommReset(void) CYSMALL \
                                                            ;
        cystatus oled_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) \
                                                        CYSMALL ;
        cystatus oled_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)  CYSMALL \
                                                            ;

        #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_oled)
            #define CyBtldrCommStart    oled_CyBtldrCommStart
            #define CyBtldrCommStop     oled_CyBtldrCommStop
            #define CyBtldrCommReset    oled_CyBtldrCommReset
            #define CyBtldrCommWrite    oled_CyBtldrCommWrite
            #define CyBtldrCommRead     oled_CyBtldrCommRead
        #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_oled) */

        /* Size of Read/Write buffers for I2C bootloader  */
        #define oled_BTLDR_SIZEOF_READ_BUFFER   (0x80u)
        #define oled_BTLDR_SIZEOF_WRITE_BUFFER  (0x80u)
        #define oled_MIN_UNT16(a, b)            ( ((uint16)(a) < (b)) ? ((uint16) (a)) : ((uint16) (b)) )
        #define oled_WAIT_1_MS                  (1u)

    #endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (oled_BOOTLOADER_INTERFACE_ENABLED) */

#endif /* (oled_MODE_SLAVE_ENABLED) */

/* Component interrupt handlers */
CY_ISR_PROTO(oled_ISR);
#if ((oled_FF_IMPLEMENTED) || (oled_WAKEUP_ENABLED))
    CY_ISR_PROTO(oled_WAKEUP_ISR);
#endif /* ((oled_FF_IMPLEMENTED) || (oled_WAKEUP_ENABLED)) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8 oled_initVar;


/***************************************
*   Initial Parameter Constants
***************************************/

#define oled_DATA_RATE          (400u)
#define oled_DEFAULT_ADDR       (8u)
#define oled_I2C_PAIR_SELECTED  (0u)

/* I2C pair enum */
#define oled_I2C_PAIR_ANY   (0x01u) /* Any pins for I2C */
#define oled_I2C_PAIR0      (0x01u) /* I2C0: (SCL = P12[4]) && (SCL = P12[5]) */
#define oled_I2C_PAIR1      (0x02u) /* I2C1: (SCL = P12[0]) && (SDA = P12[1]) */

#define oled_I2C1_SIO_PAIR  (oled_I2C_PAIR1 == oled_I2C_PAIR_SELECTED)
#define oled_I2C0_SIO_PAIR  (oled_I2C_PAIR0 == oled_I2C_PAIR_SELECTED)


/***************************************
*            API Constants
***************************************/

/* Master/Slave control constants */
#define oled_READ_XFER_MODE     (0x01u) /* Read */
#define oled_WRITE_XFER_MODE    (0x00u) /* Write */
#define oled_ACK_DATA           (0x01u) /* Send ACK */
#define oled_NAK_DATA           (0x00u) /* Send NAK */
#define oled_OVERFLOW_RETURN    (0xFFu) /* Send on bus in case of overflow */

#if (oled_MODE_MASTER_ENABLED)
    /* "Mode" constants for MasterWriteBuf() or MasterReadBuf() function */
    #define oled_MODE_COMPLETE_XFER     (0x00u) /* Full transfer with Start and Stop */
    #define oled_MODE_REPEAT_START      (0x01u) /* Begin with a ReStart instead of a Start */
    #define oled_MODE_NO_STOP           (0x02u) /* Complete the transfer without a Stop */

    /* Master status */
    #define oled_MSTAT_CLEAR            (0x00u) /* Clear (initialize) status value */

    #define oled_MSTAT_RD_CMPLT         (0x01u) /* Read complete */
    #define oled_MSTAT_WR_CMPLT         (0x02u) /* Write complete */
    #define oled_MSTAT_XFER_INP         (0x04u) /* Master transfer in progress */
    #define oled_MSTAT_XFER_HALT        (0x08u) /* Transfer is halted */

    #define oled_MSTAT_ERR_MASK         (0xF0u) /* Mask for all errors */
    #define oled_MSTAT_ERR_SHORT_XFER   (0x10u) /* Master NAKed before end of packet */
    #define oled_MSTAT_ERR_ADDR_NAK     (0x20u) /* Slave did not ACK */
    #define oled_MSTAT_ERR_ARB_LOST     (0x40u) /* Master lost arbitration during communication */
    #define oled_MSTAT_ERR_XFER         (0x80u) /* Error during transfer */

    /* Master API returns */
    #define oled_MSTR_NO_ERROR          (0x00u) /* Function complete without error */
    #define oled_MSTR_BUS_BUSY          (0x01u) /* Bus is busy, process not started */
    #define oled_MSTR_NOT_READY         (0x02u) /* Master not Master on the bus or */
                                                            /*  Slave operation in progress */
    #define oled_MSTR_ERR_LB_NAK        (0x03u) /* Last Byte Naked */
    #define oled_MSTR_ERR_ARB_LOST      (0x04u) /* Master lost arbitration during communication */
    #define oled_MSTR_ERR_ABORT_START_GEN  (0x05u) /* Master did not generate Start, the Slave */
                                                               /* was addressed before */

#endif /* (oled_MODE_MASTER_ENABLED) */

#if (oled_MODE_SLAVE_ENABLED)
    /* Slave Status Constants */
    #define oled_SSTAT_RD_CMPLT     (0x01u) /* Read transfer complete */
    #define oled_SSTAT_RD_BUSY      (0x02u) /* Read transfer in progress */
    #define oled_SSTAT_RD_ERR_OVFL  (0x04u) /* Read overflow Error */
    #define oled_SSTAT_RD_MASK      (0x0Fu) /* Read Status Mask */
    #define oled_SSTAT_RD_NO_ERR    (0x00u) /* Read no Error */

    #define oled_SSTAT_WR_CMPLT     (0x10u) /* Write transfer complete */
    #define oled_SSTAT_WR_BUSY      (0x20u) /* Write transfer in progress */
    #define oled_SSTAT_WR_ERR_OVFL  (0x40u) /* Write overflow Error */
    #define oled_SSTAT_WR_MASK      (0xF0u) /* Write Status Mask  */
    #define oled_SSTAT_WR_NO_ERR    (0x00u) /* Write no Error */

    #define oled_SSTAT_RD_CLEAR     (0x0Du) /* Read Status clear */
    #define oled_SSTAT_WR_CLEAR     (0xD0u) /* Write Status Clear */

#endif /* (oled_MODE_SLAVE_ENABLED) */


/***************************************
*       I2C state machine constants
***************************************/

/* Default slave address states */
#define  oled_SM_IDLE           (0x10u) /* Default state - IDLE */
#define  oled_SM_EXIT_IDLE      (0x00u) /* Pass master and slave processing and go to IDLE */

/* Slave mode states */
#define  oled_SM_SLAVE          (oled_SM_IDLE) /* Any Slave state */
#define  oled_SM_SL_WR_DATA     (0x11u) /* Master writes data to slave  */
#define  oled_SM_SL_RD_DATA     (0x12u) /* Master reads data from slave */

/* Master mode states */
#define  oled_SM_MASTER         (0x40u) /* Any master state */

#define  oled_SM_MSTR_RD        (0x08u) /* Any master read state          */
#define  oled_SM_MSTR_RD_ADDR   (0x49u) /* Master sends address with read */
#define  oled_SM_MSTR_RD_DATA   (0x4Au) /* Master reads data              */

#define  oled_SM_MSTR_WR        (0x04u) /* Any master read state           */
#define  oled_SM_MSTR_WR_ADDR   (0x45u) /* Master sends address with write */
#define  oled_SM_MSTR_WR_DATA   (0x46u) /* Master writes data              */

#define  oled_SM_MSTR_HALT      (0x60u) /* Master waits for ReStart */

#define oled_CHECK_SM_MASTER    (0u != (oled_SM_MASTER & oled_state))
#define oled_CHECK_SM_SLAVE     (0u != (oled_SM_SLAVE  & oled_state))


/***************************************
*              Registers
***************************************/

#if (oled_FF_IMPLEMENTED)
    /* Fixed Function registers */
    #define oled_XCFG_REG           (*(reg8 *) oled_I2C_FF__XCFG)
    #define oled_XCFG_PTR           ( (reg8 *) oled_I2C_FF__XCFG)

    #define oled_ADDR_REG           (*(reg8 *) oled_I2C_FF__ADR)
    #define oled_ADDR_PTR           ( (reg8 *) oled_I2C_FF__ADR)

    #define oled_CFG_REG            (*(reg8 *) oled_I2C_FF__CFG)
    #define oled_CFG_PTR            ( (reg8 *) oled_I2C_FF__CFG)

    #define oled_CSR_REG            (*(reg8 *) oled_I2C_FF__CSR)
    #define oled_CSR_PTR            ( (reg8 *) oled_I2C_FF__CSR)

    #define oled_DATA_REG           (*(reg8 *) oled_I2C_FF__D)
    #define oled_DATA_PTR           ( (reg8 *) oled_I2C_FF__D)

    #define oled_MCSR_REG           (*(reg8 *) oled_I2C_FF__MCSR)
    #define oled_MCSR_PTR           ( (reg8 *) oled_I2C_FF__MCSR)

    #define oled_ACT_PWRMGR_REG     (*(reg8 *) oled_I2C_FF__PM_ACT_CFG)
    #define oled_ACT_PWRMGR_PTR     ( (reg8 *) oled_I2C_FF__PM_ACT_CFG)
    #define oled_ACT_PWR_EN         ( (uint8)  oled_I2C_FF__PM_ACT_MSK)

    #define oled_STBY_PWRMGR_REG    (*(reg8 *) oled_I2C_FF__PM_STBY_CFG)
    #define oled_STBY_PWRMGR_PTR    ( (reg8 *) oled_I2C_FF__PM_STBY_CFG)
    #define oled_STBY_PWR_EN        ( (uint8)  oled_I2C_FF__PM_STBY_MSK)

    #define oled_PWRSYS_CR1_REG     (*(reg8 *) CYREG_PWRSYS_CR1)
    #define oled_PWRSYS_CR1_PTR     ( (reg8 *) CYREG_PWRSYS_CR1)

    #define oled_CLKDIV1_REG    (*(reg8 *) oled_I2C_FF__CLK_DIV1)
    #define oled_CLKDIV1_PTR    ( (reg8 *) oled_I2C_FF__CLK_DIV1)

    #define oled_CLKDIV2_REG    (*(reg8 *) oled_I2C_FF__CLK_DIV2)
    #define oled_CLKDIV2_PTR    ( (reg8 *) oled_I2C_FF__CLK_DIV2)

#else
    /* UDB implementation registers */
    #define oled_CFG_REG \
            (*(reg8 *) oled_bI2C_UDB_SyncCtl_CtrlReg__CONTROL_REG)
    #define oled_CFG_PTR \
            ( (reg8 *) oled_bI2C_UDB_SyncCtl_CtrlReg__CONTROL_REG)

    #define oled_CSR_REG        (*(reg8 *) oled_bI2C_UDB_StsReg__STATUS_REG)
    #define oled_CSR_PTR        ( (reg8 *) oled_bI2C_UDB_StsReg__STATUS_REG)

    #define oled_INT_MASK_REG   (*(reg8 *) oled_bI2C_UDB_StsReg__MASK_REG)
    #define oled_INT_MASK_PTR   ( (reg8 *) oled_bI2C_UDB_StsReg__MASK_REG)

    #define oled_INT_ENABLE_REG (*(reg8 *) oled_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)
    #define oled_INT_ENABLE_PTR ( (reg8 *) oled_bI2C_UDB_StsReg__STATUS_AUX_CTL_REG)

    #define oled_DATA_REG       (*(reg8 *) oled_bI2C_UDB_Shifter_u0__A0_REG)
    #define oled_DATA_PTR       ( (reg8 *) oled_bI2C_UDB_Shifter_u0__A0_REG)

    #define oled_GO_REG         (*(reg8 *) oled_bI2C_UDB_Shifter_u0__F1_REG)
    #define oled_GO_PTR         ( (reg8 *) oled_bI2C_UDB_Shifter_u0__F1_REG)

    #define oled_GO_DONE_REG    (*(reg8 *) oled_bI2C_UDB_Shifter_u0__A1_REG)
    #define oled_GO_DONE_PTR    ( (reg8 *) oled_bI2C_UDB_Shifter_u0__A1_REG)

    #define oled_MCLK_PRD_REG   (*(reg8 *) oled_bI2C_UDB_Master_ClkGen_u0__D0_REG)
    #define oled_MCLK_PRD_PTR   ( (reg8 *) oled_bI2C_UDB_Master_ClkGen_u0__D0_REG)

    #define oled_MCLK_CMP_REG   (*(reg8 *) oled_bI2C_UDB_Master_ClkGen_u0__D1_REG)
    #define oled_MCLK_CMP_PTR   ( (reg8 *) oled_bI2C_UDB_Master_ClkGen_u0__D1_REG)

    #if (oled_MODE_SLAVE_ENABLED)
        #define oled_ADDR_REG       (*(reg8 *) oled_bI2C_UDB_Shifter_u0__D0_REG)
        #define oled_ADDR_PTR       ( (reg8 *) oled_bI2C_UDB_Shifter_u0__D0_REG)

        #define oled_PERIOD_REG     (*(reg8 *) oled_bI2C_UDB_Slave_BitCounter__PERIOD_REG)
        #define oled_PERIOD_PTR     ( (reg8 *) oled_bI2C_UDB_Slave_BitCounter__PERIOD_REG)

        #define oled_COUNTER_REG    (*(reg8 *) oled_bI2C_UDB_Slave_BitCounter__COUNT_REG)
        #define oled_COUNTER_PTR    ( (reg8 *) oled_bI2C_UDB_Slave_BitCounter__COUNT_REG)

        #define oled_COUNTER_AUX_CTL_REG \
                                    (*(reg8 *) oled_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)
        #define oled_COUNTER_AUX_CTL_PTR \
                                    ( (reg8 *) oled_bI2C_UDB_Slave_BitCounter__CONTROL_AUX_CTL_REG)

    #endif /* (oled_MODE_SLAVE_ENABLED) */

#endif /* (oled_FF_IMPLEMENTED) */


/***************************************
*        Registers Constants
***************************************/

/* oled_I2C_IRQ */
#define oled_ISR_NUMBER     ((uint8) oled_I2C_IRQ__INTC_NUMBER)
#define oled_ISR_PRIORITY   ((uint8) oled_I2C_IRQ__INTC_PRIOR_NUM)

/* I2C Slave Data Register */
#define oled_SLAVE_ADDR_MASK    (0x7Fu)
#define oled_SLAVE_ADDR_SHIFT   (0x01u)
#define oled_DATA_MASK          (0xFFu)
#define oled_READ_FLAG          (0x01u)

/* Block reset constants */
#define oled_CLEAR_REG          (0x00u)
#define oled_BLOCK_RESET_DELAY  (2u)
#define oled_FF_RESET_DELAY     (oled_BLOCK_RESET_DELAY)
#define oled_RESTORE_TIMEOUT    (255u)

#if (oled_FF_IMPLEMENTED)
    /* XCFG I2C Extended Configuration Register */
    #define oled_XCFG_CLK_EN        (0x80u) /* Enable gated clock to block */
    #define oled_XCFG_I2C_ON        (0x40u) /* Enable I2C as wake up source*/
    #define oled_XCFG_RDY_TO_SLEEP  (0x20u) /* I2C ready go to sleep */
    #define oled_XCFG_FORCE_NACK    (0x10u) /* Force NACK all incoming transactions */
    #define oled_XCFG_NO_BC_INT     (0x08u) /* No interrupt on byte complete */
    #define oled_XCFG_BUF_MODE      (0x02u) /* Enable buffer mode */
    #define oled_XCFG_HDWR_ADDR_EN  (0x01u) /* Enable Hardware address match */

    /* CFG I2C Configuration Register */
    #define oled_CFG_SIO_SELECT     (0x80u) /* Pin Select for SCL/SDA lines */
    #define oled_CFG_PSELECT        (0x40u) /* Pin Select */
    #define oled_CFG_BUS_ERR_IE     (0x20u) /* Bus Error Interrupt Enable */
    #define oled_CFG_STOP_IE        (0x10u) /* Enable Interrupt on STOP condition */
    #define oled_CFG_CLK_RATE_MSK   (0x0Cu) /* Clock rate select */
    #define oled_CFG_CLK_RATE_100   (0x00u) /* Clock rate select 100K */
    #define oled_CFG_CLK_RATE_400   (0x04u) /* Clock rate select 400K */
    #define oled_CFG_CLK_RATE_050   (0x08u) /* Clock rate select 50K  */
    #define oled_CFG_CLK_RATE_RSVD  (0x0Cu) /* Clock rate select Invalid */
    #define oled_CFG_EN_MSTR        (0x02u) /* Enable Master operation */
    #define oled_CFG_EN_SLAVE       (0x01u) /* Enable Slave operation */

    #define oled_CFG_CLK_RATE_LESS_EQUAL_50 (0x04u) /* Clock rate select <= 50kHz */
    #define oled_CFG_CLK_RATE_GRATER_50     (0x00u) /* Clock rate select > 50kHz */

    /* CSR I2C Control and Status Register */
    #define oled_CSR_BUS_ERROR      (0x80u) /* Active high when bus error has occurred */
    #define oled_CSR_LOST_ARB       (0x40u) /* Set to 1 if lost arbitration in host mode */
    #define oled_CSR_STOP_STATUS    (0x20u) /* Set if Stop has been detected */
    #define oled_CSR_ACK            (0x10u) /* ACK response */
    #define oled_CSR_NAK            (0x00u) /* NAK response */
    #define oled_CSR_ADDRESS        (0x08u) /* Set in firmware 0 = status bit, 1 Address is slave */
    #define oled_CSR_TRANSMIT       (0x04u) /* Set in firmware 1 = transmit, 0 = receive */
    #define oled_CSR_LRB            (0x02u) /* Last received bit */
    #define oled_CSR_LRB_ACK        (0x00u) /* Last received bit was an ACK */
    #define oled_CSR_LRB_NAK        (0x02u) /* Last received bit was an NAK */
    #define oled_CSR_BYTE_COMPLETE  (0x01u) /* Informs that last byte has been sent */
    #define oled_CSR_STOP_GEN       (0x00u) /* Generate a stop condition */
    #define oled_CSR_RDY_TO_RD      (0x00u) /* Set to receive mode */

    /* MCSR I2C Master Control and Status Register */
    #define oled_MCSR_STOP_GEN      (0x10u) /* Firmware sets this bit to initiate a Stop condition */
    #define oled_MCSR_BUS_BUSY      (0x08u) /* Status bit, Set at Start and cleared at Stop condition */
    #define oled_MCSR_MSTR_MODE     (0x04u) /* Status bit, Set at Start and cleared at Stop condition */
    #define oled_MCSR_RESTART_GEN   (0x02u) /* Firmware sets this bit to initiate a ReStart condition */
    #define oled_MCSR_START_GEN     (0x01u) /* Firmware sets this bit to initiate a Start condition */

    /* PWRSYS_CR1 to handle Sleep */
    #define oled_PWRSYS_CR1_I2C_REG_BACKUP  (0x04u) /* Enables, power to I2C regs while sleep */

#else
    /* CONTROL REG bits location */
    #define oled_CTRL_START_SHIFT           (7u)
    #define oled_CTRL_STOP_SHIFT            (6u)
    #define oled_CTRL_RESTART_SHIFT         (5u)
    #define oled_CTRL_NACK_SHIFT            (4u)
    #define oled_CTRL_ANY_ADDRESS_SHIFT     (3u)
    #define oled_CTRL_TRANSMIT_SHIFT        (2u)
    #define oled_CTRL_ENABLE_MASTER_SHIFT   (1u)
    #define oled_CTRL_ENABLE_SLAVE_SHIFT    (0u)
    #define oled_CTRL_START_MASK            ((uint8) (0x01u << oled_CTRL_START_SHIFT))
    #define oled_CTRL_STOP_MASK             ((uint8) (0x01u << oled_CTRL_STOP_SHIFT))
    #define oled_CTRL_RESTART_MASK          ((uint8) (0x01u << oled_CTRL_RESTART_SHIFT))
    #define oled_CTRL_NACK_MASK             ((uint8) (0x01u << oled_CTRL_NACK_SHIFT))
    #define oled_CTRL_ANY_ADDRESS_MASK      ((uint8) (0x01u << oled_CTRL_ANY_ADDRESS_SHIFT))
    #define oled_CTRL_TRANSMIT_MASK         ((uint8) (0x01u << oled_CTRL_TRANSMIT_SHIFT))
    #define oled_CTRL_ENABLE_MASTER_MASK    ((uint8) (0x01u << oled_CTRL_ENABLE_MASTER_SHIFT))
    #define oled_CTRL_ENABLE_SLAVE_MASK     ((uint8) (0x01u << oled_CTRL_ENABLE_SLAVE_SHIFT))

    /* STATUS REG bits location */
    #define oled_STS_LOST_ARB_SHIFT         (6u)
    #define oled_STS_STOP_SHIFT             (5u)
    #define oled_STS_BUSY_SHIFT             (4u)
    #define oled_STS_ADDR_SHIFT             (3u)
    #define oled_STS_MASTER_MODE_SHIFT      (2u)
    #define oled_STS_LRB_SHIFT              (1u)
    #define oled_STS_BYTE_COMPLETE_SHIFT    (0u)
    #define oled_STS_LOST_ARB_MASK          ((uint8) (0x01u << oled_STS_LOST_ARB_SHIFT))
    #define oled_STS_STOP_MASK              ((uint8) (0x01u << oled_STS_STOP_SHIFT))
    #define oled_STS_BUSY_MASK              ((uint8) (0x01u << oled_STS_BUSY_SHIFT))
    #define oled_STS_ADDR_MASK              ((uint8) (0x01u << oled_STS_ADDR_SHIFT))
    #define oled_STS_MASTER_MODE_MASK       ((uint8) (0x01u << oled_STS_MASTER_MODE_SHIFT))
    #define oled_STS_LRB_MASK               ((uint8) (0x01u << oled_STS_LRB_SHIFT))
    #define oled_STS_BYTE_COMPLETE_MASK     ((uint8) (0x01u << oled_STS_BYTE_COMPLETE_SHIFT))

    /* AUX_CTL bits definition */
    #define oled_COUNTER_ENABLE_MASK        (0x20u) /* Enable 7-bit counter */
    #define oled_INT_ENABLE_MASK            (0x10u) /* Enable interrupt from status register */
    #define oled_CNT7_ENABLE                (oled_COUNTER_ENABLE_MASK)
    #define oled_INTR_ENABLE                (oled_INT_ENABLE_MASK)

#endif /* (oled_FF_IMPLEMENTED) */


/***************************************
*        Marco
***************************************/

/* ACK and NACK for data and address checks */
#define oled_CHECK_ADDR_ACK(csr)    ((oled_CSR_LRB_ACK | oled_CSR_ADDRESS) == \
                                                 ((oled_CSR_LRB    | oled_CSR_ADDRESS) &  \
                                                  (csr)))


#define oled_CHECK_ADDR_NAK(csr)    ((oled_CSR_LRB_NAK | oled_CSR_ADDRESS) == \
                                                 ((oled_CSR_LRB    | oled_CSR_ADDRESS) &  \
                                                  (csr)))

#define oled_CHECK_DATA_ACK(csr)    (0u == ((csr) & oled_CSR_LRB_NAK))

/* MCSR conditions check */
#define oled_CHECK_BUS_FREE(mcsr)       (0u == ((mcsr) & oled_MCSR_BUS_BUSY))
#define oled_CHECK_MASTER_MODE(mcsr)    (0u != ((mcsr) & oled_MCSR_MSTR_MODE))

/* CSR conditions check */
#define oled_WAIT_BYTE_COMPLETE(csr)    (0u == ((csr) & oled_CSR_BYTE_COMPLETE))
#define oled_WAIT_STOP_COMPLETE(csr)    (0u == ((csr) & (oled_CSR_BYTE_COMPLETE | \
                                                                     oled_CSR_STOP_STATUS)))
#define oled_CHECK_BYTE_COMPLETE(csr)   (0u != ((csr) & oled_CSR_BYTE_COMPLETE))
#define oled_CHECK_STOP_STS(csr)        (0u != ((csr) & oled_CSR_STOP_STATUS))
#define oled_CHECK_LOST_ARB(csr)        (0u != ((csr) & oled_CSR_LOST_ARB))
#define oled_CHECK_ADDRESS_STS(csr)     (0u != ((csr) & oled_CSR_ADDRESS))

/* Software start and end of transaction check */
#define oled_CHECK_RESTART(mstrCtrl)    (0u != ((mstrCtrl) & oled_MODE_REPEAT_START))
#define oled_CHECK_NO_STOP(mstrCtrl)    (0u != ((mstrCtrl) & oled_MODE_NO_STOP))

/* Send read or write completion depends on state */
#define oled_GET_MSTAT_CMPLT ((0u != (oled_state & oled_SM_MSTR_RD)) ? \
                                                 (oled_MSTAT_RD_CMPLT) : (oled_MSTAT_WR_CMPLT))

/* Returns 7-bit slave address */
#define oled_GET_SLAVE_ADDR(dataReg)   (((dataReg) >> oled_SLAVE_ADDR_SHIFT) & \
                                                                  oled_SLAVE_ADDR_MASK)

#if (oled_FF_IMPLEMENTED)
    /* Check enable of module */
    #define oled_I2C_ENABLE_REG     (oled_ACT_PWRMGR_REG)
    #define oled_IS_I2C_ENABLE(reg) (0u != ((reg) & oled_ACT_PWR_EN))
    #define oled_IS_ENABLED         (0u != (oled_ACT_PWRMGR_REG & oled_ACT_PWR_EN))

    #define oled_CHECK_PWRSYS_I2C_BACKUP    (0u != (oled_PWRSYS_CR1_I2C_REG_BACKUP & \
                                                                oled_PWRSYS_CR1_REG))

    /* Check start condition generation */
    #define oled_CHECK_START_GEN(mcsr)  ((0u != ((mcsr) & oled_MCSR_START_GEN)) && \
                                                     (0u == ((mcsr) & oled_MCSR_MSTR_MODE)))

    #define oled_CLEAR_START_GEN        do{ \
                                                        oled_MCSR_REG &=                                   \
                                                                           ((uint8) ~oled_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define oled_ENABLE_INT_ON_STOP     do{ \
                                                        oled_CFG_REG |= oled_CFG_STOP_IE; \
                                                    }while(0)

    #define oled_DISABLE_INT_ON_STOP    do{ \
                                                        oled_CFG_REG &=                                 \
                                                                           ((uint8) ~oled_CFG_STOP_IE); \
                                                    }while(0)

    /* Transmit data */
    #define oled_TRANSMIT_DATA          do{ \
                                                        oled_CSR_REG = oled_CSR_TRANSMIT; \
                                                    }while(0)

    #define oled_ACK_AND_TRANSMIT       do{ \
                                                        oled_CSR_REG = (oled_CSR_ACK |      \
                                                                                    oled_CSR_TRANSMIT); \
                                                    }while(0)

    #define oled_NAK_AND_TRANSMIT       do{ \
                                                        oled_CSR_REG = oled_CSR_NAK; \
                                                    }while(0)

    /* Special case: udb needs to ack, ff needs to nak */
    #define oled_ACKNAK_AND_TRANSMIT    do{ \
                                                        oled_CSR_REG  = (oled_CSR_NAK |      \
                                                                                     oled_CSR_TRANSMIT); \
                                                    }while(0)
    /* Receive data */
    #define oled_ACK_AND_RECEIVE        do{ \
                                                        oled_CSR_REG = oled_CSR_ACK; \
                                                    }while(0)

    #define oled_NAK_AND_RECEIVE        do{ \
                                                        oled_CSR_REG = oled_CSR_NAK; \
                                                    }while(0)

    #define oled_READY_TO_READ          do{ \
                                                        oled_CSR_REG = oled_CSR_RDY_TO_RD; \
                                                    }while(0)

    /* Release bus after lost arbitration */
    #define oled_BUS_RELEASE    oled_READY_TO_READ

    /* Master Start/ReStart/Stop conditions generation */
    #define oled_GENERATE_START         do{ \
                                                        oled_MCSR_REG = oled_MCSR_START_GEN; \
                                                    }while(0)

    #define oled_GENERATE_RESTART \
                    do{                       \
                        oled_MCSR_REG = (oled_MCSR_RESTART_GEN | \
                                                     oled_MCSR_STOP_GEN);    \
                        oled_CSR_REG  = oled_CSR_TRANSMIT;       \
                    }while(0)

    #define oled_GENERATE_STOP \
                    do{                    \
                        oled_MCSR_REG = oled_MCSR_STOP_GEN; \
                        oled_CSR_REG  = oled_CSR_TRANSMIT;  \
                    }while(0)

    /* Master manual APIs compatible defines */
    #define oled_GENERATE_START_MANUAL      oled_GENERATE_START
    #define oled_GENERATE_RESTART_MANUAL    oled_GENERATE_RESTART
    #define oled_GENERATE_STOP_MANUAL       oled_GENERATE_STOP
    #define oled_TRANSMIT_DATA_MANUAL       oled_TRANSMIT_DATA
    #define oled_READY_TO_READ_MANUAL       oled_READY_TO_READ
    #define oled_ACK_AND_RECEIVE_MANUAL     oled_ACK_AND_RECEIVE
    #define oled_BUS_RELEASE_MANUAL         oled_BUS_RELEASE

#else

    /* Masks to enable interrupts from Status register */
    #define oled_STOP_IE_MASK           (oled_STS_STOP_MASK)
    #define oled_BYTE_COMPLETE_IE_MASK  (oled_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: CSR register bit-fields */
    #define oled_CSR_LOST_ARB       (oled_STS_LOST_ARB_MASK)
    #define oled_CSR_STOP_STATUS    (oled_STS_STOP_MASK)
    #define oled_CSR_BUS_ERROR      (0x00u)
    #define oled_CSR_ADDRESS        (oled_STS_ADDR_MASK)
    #define oled_CSR_TRANSMIT       (oled_CTRL_TRANSMIT_MASK)
    #define oled_CSR_LRB            (oled_STS_LRB_MASK)
    #define oled_CSR_LRB_NAK        (oled_STS_LRB_MASK)
    #define oled_CSR_LRB_ACK        (0x00u)
    #define oled_CSR_BYTE_COMPLETE  (oled_STS_BYTE_COMPLETE_MASK)

    /* FF compatibility: MCSR registers bit-fields */
    #define oled_MCSR_REG           (oled_CSR_REG)  /* UDB incorporates master and slave regs */
    #define oled_MCSR_BUS_BUSY      (oled_STS_BUSY_MASK)      /* Is bus is busy               */
    #define oled_MCSR_START_GEN     (oled_CTRL_START_MASK)    /* Generate Start condition     */
    #define oled_MCSR_RESTART_GEN   (oled_CTRL_RESTART_MASK)  /* Generates RESTART condition  */
    #define oled_MCSR_MSTR_MODE     (oled_STS_MASTER_MODE_MASK)/* Define if active Master     */

    /* Data to write into TX FIFO to release FSM */
    #define oled_PREPARE_TO_RELEASE (0xFFu)
    #define oled_RELEASE_FSM        (0x00u)

    /* Define release command done: history of byte complete status is cleared */
    #define oled_WAIT_RELEASE_CMD_DONE  (oled_RELEASE_FSM != oled_GO_DONE_REG)

    /* Check enable of module */
    #define oled_I2C_ENABLE_REG     (oled_CFG_REG)
    #define oled_IS_I2C_ENABLE(reg) ((0u != ((reg) & oled_ENABLE_MASTER)) || \
                                                 (0u != ((reg) & oled_ENABLE_SLAVE)))

    #define oled_IS_ENABLED         (0u != (oled_CFG_REG & oled_ENABLE_MS))

    /* Check start condition generation */
    #define oled_CHECK_START_GEN(mcsr)  ((0u != (oled_CFG_REG &        \
                                                             oled_MCSR_START_GEN)) \
                                                    &&                                         \
                                                    (0u == ((mcsr) & oled_MCSR_MSTR_MODE)))

    #define oled_CLEAR_START_GEN        do{ \
                                                        oled_CFG_REG &=                 \
                                                        ((uint8) ~oled_MCSR_START_GEN); \
                                                    }while(0)

    /* Stop interrupt */
    #define oled_ENABLE_INT_ON_STOP     do{ \
                                                       oled_INT_MASK_REG |= oled_STOP_IE_MASK; \
                                                    }while(0)

    #define oled_DISABLE_INT_ON_STOP    do{ \
                                                        oled_INT_MASK_REG &=                               \
                                                                             ((uint8) ~oled_STOP_IE_MASK); \
                                                    }while(0)

    /* Transmit data */
    #define oled_TRANSMIT_DATA \
                                    do{    \
                                        oled_CFG_REG     = (oled_CTRL_TRANSMIT_MASK | \
                                                                       oled_CTRL_DEFAULT);        \
                                        oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE;   \
                                        oled_GO_REG      = oled_RELEASE_FSM;          \
                                    }while(0)

    #define oled_ACK_AND_TRANSMIT   oled_TRANSMIT_DATA

    #define oled_NAK_AND_TRANSMIT \
                                        do{   \
                                            oled_CFG_REG     = (oled_CTRL_NACK_MASK     | \
                                                                            oled_CTRL_TRANSMIT_MASK | \
                                                                            oled_CTRL_DEFAULT);       \
                                            oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE;   \
                                            oled_GO_REG      = oled_RELEASE_FSM;          \
                                        }while(0)

    /* Receive data */
    #define oled_READY_TO_READ  \
                                        do{ \
                                            oled_CFG_REG     = oled_CTRL_DEFAULT;       \
                                            oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE; \
                                            oled_GO_REG      = oled_RELEASE_FSM;       \
                                        }while(0)

    #define oled_ACK_AND_RECEIVE    oled_READY_TO_READ

    /* Release bus after arbitration is lost */
    #define oled_BUS_RELEASE    oled_READY_TO_READ

    #define oled_NAK_AND_RECEIVE \
                                        do{  \
                                            oled_CFG_REG     = (oled_CTRL_NACK_MASK |   \
                                                                            oled_CTRL_DEFAULT);     \
                                            oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE; \
                                            oled_GO_REG      = oled_RELEASE_FSM;       \
                                        }while(0)

    /* Master condition generation */
    #define oled_GENERATE_START \
                                        do{ \
                                            oled_CFG_REG     = (oled_CTRL_START_MASK |  \
                                                                            oled_CTRL_DEFAULT);     \
                                            oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE; \
                                            oled_GO_REG      = oled_RELEASE_FSM;       \
                                        }while(0)

    #define oled_GENERATE_RESTART \
                                        do{   \
                                            oled_CFG_REG     = (oled_CTRL_RESTART_MASK | \
                                                                            oled_CTRL_NACK_MASK    | \
                                                                            oled_CTRL_DEFAULT);      \
                                            oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE;  \
                                            oled_GO_REG  =     oled_RELEASE_FSM;         \
                                        }while(0)

    #define oled_GENERATE_STOP  \
                                        do{ \
                                            oled_CFG_REG    = (oled_CTRL_NACK_MASK |    \
                                                                           oled_CTRL_STOP_MASK |    \
                                                                           oled_CTRL_DEFAULT);      \
                                            oled_GO_DONE_REG = oled_PREPARE_TO_RELEASE; \
                                            oled_GO_REG      = oled_RELEASE_FSM;        \
                                        }while(0)

    /* Master manual APIs compatible macros */
    /* These macros wait until byte complete history is cleared after command issued */
    #define oled_GENERATE_START_MANUAL \
                                        do{ \
                                            oled_GENERATE_START;                  \
                                            /* Wait until byte complete history is cleared */ \
                                            while(oled_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)
                                        
    #define oled_GENERATE_RESTART_MANUAL \
                                        do{          \
                                            oled_GENERATE_RESTART;                \
                                            /* Wait until byte complete history is cleared */ \
                                            while(oled_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define oled_GENERATE_STOP_MANUAL \
                                        do{       \
                                            oled_GENERATE_STOP;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(oled_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define oled_TRANSMIT_DATA_MANUAL \
                                        do{       \
                                            oled_TRANSMIT_DATA;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(oled_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define oled_READY_TO_READ_MANUAL \
                                        do{       \
                                            oled_READY_TO_READ;                   \
                                            /* Wait until byte complete history is cleared */ \
                                            while(oled_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define oled_ACK_AND_RECEIVE_MANUAL \
                                        do{         \
                                            oled_ACK_AND_RECEIVE;                 \
                                            /* Wait until byte complete history is cleared */ \
                                            while(oled_WAIT_RELEASE_CMD_DONE)     \
                                            {                                                 \
                                            }                                                 \
                                        }while(0)

    #define oled_BUS_RELEASE_MANUAL oled_READY_TO_READ_MANUAL

#endif /* (oled_FF_IMPLEMENTED) */


/***************************************
*     Default register init constants
***************************************/

#define oled_DISABLE    (0u)
#define oled_ENABLE     (1u)

#if (oled_FF_IMPLEMENTED)
    /* oled_XCFG_REG: bits definition */
    #define oled_DEFAULT_XCFG_HW_ADDR_EN ((oled_HW_ADRR_DECODE) ? \
                                                        (oled_XCFG_HDWR_ADDR_EN) : (0u))

    #define oled_DEFAULT_XCFG_I2C_ON    ((oled_WAKEUP_ENABLED) ? \
                                                        (oled_XCFG_I2C_ON) : (0u))


    #define oled_DEFAULT_CFG_SIO_SELECT ((oled_I2C1_SIO_PAIR) ? \
                                                        (oled_CFG_SIO_SELECT) : (0u))


    /* oled_CFG_REG: bits definition */
    #define oled_DEFAULT_CFG_PSELECT    ((oled_WAKEUP_ENABLED) ? \
                                                        (oled_CFG_PSELECT) : (0u))

    #define oled_DEFAULT_CLK_RATE0  ((oled_DATA_RATE <= 50u) ?        \
                                                    (oled_CFG_CLK_RATE_050) :     \
                                                    ((oled_DATA_RATE <= 100u) ?   \
                                                        (oled_CFG_CLK_RATE_100) : \
                                                        (oled_CFG_CLK_RATE_400)))

    #define oled_DEFAULT_CLK_RATE1  ((oled_DATA_RATE <= 50u) ?           \
                                                 (oled_CFG_CLK_RATE_LESS_EQUAL_50) : \
                                                 (oled_CFG_CLK_RATE_GRATER_50))

    #define oled_DEFAULT_CLK_RATE   (oled_DEFAULT_CLK_RATE1)


    #define oled_ENABLE_MASTER      ((oled_MODE_MASTER_ENABLED) ? \
                                                 (oled_CFG_EN_MSTR) : (0u))

    #define oled_ENABLE_SLAVE       ((oled_MODE_SLAVE_ENABLED) ? \
                                                 (oled_CFG_EN_SLAVE) : (0u))

    #define oled_ENABLE_MS      (oled_ENABLE_MASTER | oled_ENABLE_SLAVE)


    /* oled_DEFAULT_XCFG_REG */
    #define oled_DEFAULT_XCFG   (oled_XCFG_CLK_EN         | \
                                             oled_DEFAULT_XCFG_I2C_ON | \
                                             oled_DEFAULT_XCFG_HW_ADDR_EN)

    /* oled_DEFAULT_CFG_REG */
    #define oled_DEFAULT_CFG    (oled_DEFAULT_CFG_SIO_SELECT | \
                                             oled_DEFAULT_CFG_PSELECT    | \
                                             oled_DEFAULT_CLK_RATE       | \
                                             oled_ENABLE_MASTER          | \
                                             oled_ENABLE_SLAVE)

    /*oled_DEFAULT_DIVIDE_FACTOR_REG */
    #define oled_DEFAULT_DIVIDE_FACTOR  ((uint16) 13u)

#else
    /* oled_CFG_REG: bits definition  */
    #define oled_ENABLE_MASTER  ((oled_MODE_MASTER_ENABLED) ? \
                                             (oled_CTRL_ENABLE_MASTER_MASK) : (0u))

    #define oled_ENABLE_SLAVE   ((oled_MODE_SLAVE_ENABLED) ? \
                                             (oled_CTRL_ENABLE_SLAVE_MASK) : (0u))

    #define oled_ENABLE_MS      (oled_ENABLE_MASTER | oled_ENABLE_SLAVE)


    #define oled_DEFAULT_CTRL_ANY_ADDR   ((oled_HW_ADRR_DECODE) ? \
                                                      (0u) : (oled_CTRL_ANY_ADDRESS_MASK))

    /* oled_DEFAULT_CFG_REG */
    #define oled_DEFAULT_CFG    (oled_DEFAULT_CTRL_ANY_ADDR)

    /* All CTRL default bits to be used in macro */
    #define oled_CTRL_DEFAULT   (oled_DEFAULT_CTRL_ANY_ADDR | oled_ENABLE_MS)

    /* Master clock generator: d0 and d1 */
    #define oled_MCLK_PERIOD_VALUE  (0x0Fu)
    #define oled_MCLK_COMPARE_VALUE (0x08u)

    /* Slave bit-counter: control period */
    #define oled_PERIOD_VALUE       (0x07u)

    /* oled_DEFAULT_INT_MASK */
    #define oled_DEFAULT_INT_MASK   (oled_BYTE_COMPLETE_IE_MASK)

    /* oled_DEFAULT_MCLK_PRD_REG */
    #define oled_DEFAULT_MCLK_PRD   (oled_MCLK_PERIOD_VALUE)

    /* oled_DEFAULT_MCLK_CMP_REG */
    #define oled_DEFAULT_MCLK_CMP   (oled_MCLK_COMPARE_VALUE)

    /* oled_DEFAULT_PERIOD_REG */
    #define oled_DEFAULT_PERIOD     (oled_PERIOD_VALUE)

#endif /* (oled_FF_IMPLEMENTED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

#define oled_SSTAT_RD_ERR       (0x08u)
#define oled_SSTAT_WR_ERR       (0x80u)
#define oled_MSTR_SLAVE_BUSY    (oled_MSTR_NOT_READY)
#define oled_MSTAT_ERR_BUF_OVFL (0x80u)
#define oled_SSTAT_RD_CMPT      (oled_SSTAT_RD_CMPLT)
#define oled_SSTAT_WR_CMPT      (oled_SSTAT_WR_CMPLT)
#define oled_MODE_MULTI_MASTER_ENABLE    (oled_MODE_MULTI_MASTER_MASK)
#define oled_DATA_RATE_50       (50u)
#define oled_DATA_RATE_100      (100u)
#define oled_DEV_MASK           (0xF0u)
#define oled_SM_SL_STOP         (0x14u)
#define oled_SM_MASTER_IDLE     (0x40u)
#define oled_HDWR_DECODE        (0x01u)

#endif /* CY_I2C_oled_H */


/* [] END OF FILE */
