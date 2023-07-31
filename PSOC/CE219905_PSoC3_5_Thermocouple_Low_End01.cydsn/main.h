/******************************************************************************
* File Name: main.h
*  Version 1.0
*
* Description:
*  This file contains defines and function prototype for temperature measurement 
*  using thermocouple
*
* Note:
*
********************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/
#ifndef MAIN_H
#define MAIN_H
#include "project.h"
#include <stdio.h>

/* Defines for ADC MUX channels */
#define TC_HOT_JN_CHANNEL               0
#define IC_CHANNEL              		1
#define THERM_SIGNAL_CHANNEL			2
#define THERM_REF_CHANNEL				3
#define OFFSET_CHANNEL            		4

/* Filter coefficient for sensors */
#define MAX_FILTER_COEFF_SHIFT			8
#define TC_FILTER_COEFF_SHIFT           6

/* Filter Feed Forward Term. It is set equal to 640 ADC counts or 2°C.
 * 256 (2^MAX_FILTER_COEFF_SHIFT) is to make the comparison fair */
#define FILTER_FEEDFORWARD				((int32)640 * 256)

/* Zero degree offset voltage of DS600(509 mV from sensor datasheet) */
#define ZERO_DEGREE_OFFSET_VOLTAGE      509000

/* Temperature coefficient for DS600(6.45 mV/°C from sensor datasheet) */
#define VOLTAGE_PER_DEGREE              645
#define SCALING							10

/* Voltage threshold (in uV) based on which broken thermocouple alert is made*/
#define BROKEN_TC_THRESHOLD				-10000

/* Function to initialize components */
void InitializeComponents(void);

/* Function to measure cold junction temperature */
int32 MeasureColdJnSensorTemp(void);

/* Funtion to measure IC temperature */
int32 MeasureICTemp(void);

/* Funtion to measure thermistor temperature */
int32 MeasureThermistorTemp(void);

/* Function prototype for IIR filter */
int32 FilterSignal(int32 ADCSample);

/* Function prototype to measure offset voltage */
int32 MeasureOffset(void);

/*Interrupt handler function*/
CY_ISR_PROTO(switchISRhandler);

#endif

/* [] END OF FILE */
