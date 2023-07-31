/*******************************************************************************
* File Name: DOWN_BUTTON.h  
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

#if !defined(CY_PINS_DOWN_BUTTON_H) /* Pins DOWN_BUTTON_H */
#define CY_PINS_DOWN_BUTTON_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "DOWN_BUTTON_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 DOWN_BUTTON__PORT == 15 && ((DOWN_BUTTON__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    DOWN_BUTTON_Write(uint8 value);
void    DOWN_BUTTON_SetDriveMode(uint8 mode);
uint8   DOWN_BUTTON_ReadDataReg(void);
uint8   DOWN_BUTTON_Read(void);
void    DOWN_BUTTON_SetInterruptMode(uint16 position, uint16 mode);
uint8   DOWN_BUTTON_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the DOWN_BUTTON_SetDriveMode() function.
     *  @{
     */
        #define DOWN_BUTTON_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define DOWN_BUTTON_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define DOWN_BUTTON_DM_RES_UP          PIN_DM_RES_UP
        #define DOWN_BUTTON_DM_RES_DWN         PIN_DM_RES_DWN
        #define DOWN_BUTTON_DM_OD_LO           PIN_DM_OD_LO
        #define DOWN_BUTTON_DM_OD_HI           PIN_DM_OD_HI
        #define DOWN_BUTTON_DM_STRONG          PIN_DM_STRONG
        #define DOWN_BUTTON_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define DOWN_BUTTON_MASK               DOWN_BUTTON__MASK
#define DOWN_BUTTON_SHIFT              DOWN_BUTTON__SHIFT
#define DOWN_BUTTON_WIDTH              1u

/* Interrupt constants */
#if defined(DOWN_BUTTON__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in DOWN_BUTTON_SetInterruptMode() function.
     *  @{
     */
        #define DOWN_BUTTON_INTR_NONE      (uint16)(0x0000u)
        #define DOWN_BUTTON_INTR_RISING    (uint16)(0x0001u)
        #define DOWN_BUTTON_INTR_FALLING   (uint16)(0x0002u)
        #define DOWN_BUTTON_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define DOWN_BUTTON_INTR_MASK      (0x01u) 
#endif /* (DOWN_BUTTON__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define DOWN_BUTTON_PS                     (* (reg8 *) DOWN_BUTTON__PS)
/* Data Register */
#define DOWN_BUTTON_DR                     (* (reg8 *) DOWN_BUTTON__DR)
/* Port Number */
#define DOWN_BUTTON_PRT_NUM                (* (reg8 *) DOWN_BUTTON__PRT) 
/* Connect to Analog Globals */                                                  
#define DOWN_BUTTON_AG                     (* (reg8 *) DOWN_BUTTON__AG)                       
/* Analog MUX bux enable */
#define DOWN_BUTTON_AMUX                   (* (reg8 *) DOWN_BUTTON__AMUX) 
/* Bidirectional Enable */                                                        
#define DOWN_BUTTON_BIE                    (* (reg8 *) DOWN_BUTTON__BIE)
/* Bit-mask for Aliased Register Access */
#define DOWN_BUTTON_BIT_MASK               (* (reg8 *) DOWN_BUTTON__BIT_MASK)
/* Bypass Enable */
#define DOWN_BUTTON_BYP                    (* (reg8 *) DOWN_BUTTON__BYP)
/* Port wide control signals */                                                   
#define DOWN_BUTTON_CTL                    (* (reg8 *) DOWN_BUTTON__CTL)
/* Drive Modes */
#define DOWN_BUTTON_DM0                    (* (reg8 *) DOWN_BUTTON__DM0) 
#define DOWN_BUTTON_DM1                    (* (reg8 *) DOWN_BUTTON__DM1)
#define DOWN_BUTTON_DM2                    (* (reg8 *) DOWN_BUTTON__DM2) 
/* Input Buffer Disable Override */
#define DOWN_BUTTON_INP_DIS                (* (reg8 *) DOWN_BUTTON__INP_DIS)
/* LCD Common or Segment Drive */
#define DOWN_BUTTON_LCD_COM_SEG            (* (reg8 *) DOWN_BUTTON__LCD_COM_SEG)
/* Enable Segment LCD */
#define DOWN_BUTTON_LCD_EN                 (* (reg8 *) DOWN_BUTTON__LCD_EN)
/* Slew Rate Control */
#define DOWN_BUTTON_SLW                    (* (reg8 *) DOWN_BUTTON__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define DOWN_BUTTON_PRTDSI__CAPS_SEL       (* (reg8 *) DOWN_BUTTON__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define DOWN_BUTTON_PRTDSI__DBL_SYNC_IN    (* (reg8 *) DOWN_BUTTON__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define DOWN_BUTTON_PRTDSI__OE_SEL0        (* (reg8 *) DOWN_BUTTON__PRTDSI__OE_SEL0) 
#define DOWN_BUTTON_PRTDSI__OE_SEL1        (* (reg8 *) DOWN_BUTTON__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define DOWN_BUTTON_PRTDSI__OUT_SEL0       (* (reg8 *) DOWN_BUTTON__PRTDSI__OUT_SEL0) 
#define DOWN_BUTTON_PRTDSI__OUT_SEL1       (* (reg8 *) DOWN_BUTTON__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define DOWN_BUTTON_PRTDSI__SYNC_OUT       (* (reg8 *) DOWN_BUTTON__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(DOWN_BUTTON__SIO_CFG)
    #define DOWN_BUTTON_SIO_HYST_EN        (* (reg8 *) DOWN_BUTTON__SIO_HYST_EN)
    #define DOWN_BUTTON_SIO_REG_HIFREQ     (* (reg8 *) DOWN_BUTTON__SIO_REG_HIFREQ)
    #define DOWN_BUTTON_SIO_CFG            (* (reg8 *) DOWN_BUTTON__SIO_CFG)
    #define DOWN_BUTTON_SIO_DIFF           (* (reg8 *) DOWN_BUTTON__SIO_DIFF)
#endif /* (DOWN_BUTTON__SIO_CFG) */

/* Interrupt Registers */
#if defined(DOWN_BUTTON__INTSTAT)
    #define DOWN_BUTTON_INTSTAT            (* (reg8 *) DOWN_BUTTON__INTSTAT)
    #define DOWN_BUTTON_SNAP               (* (reg8 *) DOWN_BUTTON__SNAP)
    
	#define DOWN_BUTTON_0_INTTYPE_REG 		(* (reg8 *) DOWN_BUTTON__0__INTTYPE)
#endif /* (DOWN_BUTTON__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_DOWN_BUTTON_H */


/* [] END OF FILE */
