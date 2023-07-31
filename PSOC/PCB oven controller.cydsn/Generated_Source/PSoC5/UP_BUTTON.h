/*******************************************************************************
* File Name: UP_BUTTON.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_UP_BUTTON_H) /* Pins UP_BUTTON_H */
#define CY_PINS_UP_BUTTON_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "UP_BUTTON_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 UP_BUTTON__PORT == 15 && ((UP_BUTTON__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    UP_BUTTON_Write(uint8 value);
void    UP_BUTTON_SetDriveMode(uint8 mode);
uint8   UP_BUTTON_ReadDataReg(void);
uint8   UP_BUTTON_Read(void);
void    UP_BUTTON_SetInterruptMode(uint16 position, uint16 mode);
uint8   UP_BUTTON_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the UP_BUTTON_SetDriveMode() function.
     *  @{
     */
        #define UP_BUTTON_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define UP_BUTTON_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define UP_BUTTON_DM_RES_UP          PIN_DM_RES_UP
        #define UP_BUTTON_DM_RES_DWN         PIN_DM_RES_DWN
        #define UP_BUTTON_DM_OD_LO           PIN_DM_OD_LO
        #define UP_BUTTON_DM_OD_HI           PIN_DM_OD_HI
        #define UP_BUTTON_DM_STRONG          PIN_DM_STRONG
        #define UP_BUTTON_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define UP_BUTTON_MASK               UP_BUTTON__MASK
#define UP_BUTTON_SHIFT              UP_BUTTON__SHIFT
#define UP_BUTTON_WIDTH              1u

/* Interrupt constants */
#if defined(UP_BUTTON__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in UP_BUTTON_SetInterruptMode() function.
     *  @{
     */
        #define UP_BUTTON_INTR_NONE      (uint16)(0x0000u)
        #define UP_BUTTON_INTR_RISING    (uint16)(0x0001u)
        #define UP_BUTTON_INTR_FALLING   (uint16)(0x0002u)
        #define UP_BUTTON_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define UP_BUTTON_INTR_MASK      (0x01u) 
#endif /* (UP_BUTTON__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define UP_BUTTON_PS                     (* (reg8 *) UP_BUTTON__PS)
/* Data Register */
#define UP_BUTTON_DR                     (* (reg8 *) UP_BUTTON__DR)
/* Port Number */
#define UP_BUTTON_PRT_NUM                (* (reg8 *) UP_BUTTON__PRT) 
/* Connect to Analog Globals */                                                  
#define UP_BUTTON_AG                     (* (reg8 *) UP_BUTTON__AG)                       
/* Analog MUX bux enable */
#define UP_BUTTON_AMUX                   (* (reg8 *) UP_BUTTON__AMUX) 
/* Bidirectional Enable */                                                        
#define UP_BUTTON_BIE                    (* (reg8 *) UP_BUTTON__BIE)
/* Bit-mask for Aliased Register Access */
#define UP_BUTTON_BIT_MASK               (* (reg8 *) UP_BUTTON__BIT_MASK)
/* Bypass Enable */
#define UP_BUTTON_BYP                    (* (reg8 *) UP_BUTTON__BYP)
/* Port wide control signals */                                                   
#define UP_BUTTON_CTL                    (* (reg8 *) UP_BUTTON__CTL)
/* Drive Modes */
#define UP_BUTTON_DM0                    (* (reg8 *) UP_BUTTON__DM0) 
#define UP_BUTTON_DM1                    (* (reg8 *) UP_BUTTON__DM1)
#define UP_BUTTON_DM2                    (* (reg8 *) UP_BUTTON__DM2) 
/* Input Buffer Disable Override */
#define UP_BUTTON_INP_DIS                (* (reg8 *) UP_BUTTON__INP_DIS)
/* LCD Common or Segment Drive */
#define UP_BUTTON_LCD_COM_SEG            (* (reg8 *) UP_BUTTON__LCD_COM_SEG)
/* Enable Segment LCD */
#define UP_BUTTON_LCD_EN                 (* (reg8 *) UP_BUTTON__LCD_EN)
/* Slew Rate Control */
#define UP_BUTTON_SLW                    (* (reg8 *) UP_BUTTON__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define UP_BUTTON_PRTDSI__CAPS_SEL       (* (reg8 *) UP_BUTTON__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define UP_BUTTON_PRTDSI__DBL_SYNC_IN    (* (reg8 *) UP_BUTTON__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define UP_BUTTON_PRTDSI__OE_SEL0        (* (reg8 *) UP_BUTTON__PRTDSI__OE_SEL0) 
#define UP_BUTTON_PRTDSI__OE_SEL1        (* (reg8 *) UP_BUTTON__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define UP_BUTTON_PRTDSI__OUT_SEL0       (* (reg8 *) UP_BUTTON__PRTDSI__OUT_SEL0) 
#define UP_BUTTON_PRTDSI__OUT_SEL1       (* (reg8 *) UP_BUTTON__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define UP_BUTTON_PRTDSI__SYNC_OUT       (* (reg8 *) UP_BUTTON__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(UP_BUTTON__SIO_CFG)
    #define UP_BUTTON_SIO_HYST_EN        (* (reg8 *) UP_BUTTON__SIO_HYST_EN)
    #define UP_BUTTON_SIO_REG_HIFREQ     (* (reg8 *) UP_BUTTON__SIO_REG_HIFREQ)
    #define UP_BUTTON_SIO_CFG            (* (reg8 *) UP_BUTTON__SIO_CFG)
    #define UP_BUTTON_SIO_DIFF           (* (reg8 *) UP_BUTTON__SIO_DIFF)
#endif /* (UP_BUTTON__SIO_CFG) */

/* Interrupt Registers */
#if defined(UP_BUTTON__INTSTAT)
    #define UP_BUTTON_INTSTAT            (* (reg8 *) UP_BUTTON__INTSTAT)
    #define UP_BUTTON_SNAP               (* (reg8 *) UP_BUTTON__SNAP)
    
	#define UP_BUTTON_0_INTTYPE_REG 		(* (reg8 *) UP_BUTTON__0__INTTYPE)
#endif /* (UP_BUTTON__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_UP_BUTTON_H */


/* [] END OF FILE */
