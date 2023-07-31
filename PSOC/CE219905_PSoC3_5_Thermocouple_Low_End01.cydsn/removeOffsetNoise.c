/******************************************************************************
* File Name: removeOffsetNoise.c
*  Version 1.0
*
* Description:
*  This file contains the code to cancel offset and filter noise
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
#include <main.h>

/*******************************************************************************
*  Function Name: MeasureOffset
********************************************************************************
* Summary:
*  This function reads the value of the ADC when the inputs are shorted
*
* Parameters:
* void:
*
* Return:
* int32: Offset of ADC 
*
* Theory:
*
* Side Effects:
* None
*******************************************************************************/
int32 MeasureOffset(void)
{
    int32 result;
     
	ADCMux_FastSelect(OFFSET_CHANNEL);
    /*No need to change ADC config, use same config as last measurement*/
	ADC_StartConvert();
	ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    result = ADC_GetResult32();
 
    ADC_StopConvert();
     
    return result;
}

/*******************************************************************************
*  Function Name: FilterSignal
********************************************************************************
* Summary:
*  This IIR filter processes the sampled ADC data. For first sample passed 
*  the function simply returns the passed ADC sample. for second sample onwards,
*  fraction of input signal based upon the filter coefficient, is calculated and 
*  added to the previously stored filtered value.
*
* Parameters:
* (int32)  ADCSample       - ADC sample read for given signal
* (int32)  filteredValue   - stored filtered value for given sensor
*
* Return: 
* (int32)filteredValue - filtered value passed is updated after applying 
* IIR filter to the ADC sample.
*
* Theory:
* For more information on the IIR filter please refer to Cypress application
* note AN2099.
*
* Side Effects:
* None
*******************************************************************************/
int32 FilterSignal(int32 ADCSample)
{
	/* Filtered value */
	static int32 filteredValue = 0;
	
	/* Filtered value rounded-off to 20-bits */
	int32 filValueRounded;
	
	/* Left shift input by 8 to allow divisions up to 256 */
	ADCSample <<= MAX_FILTER_COEFF_SHIFT;
	
	/*feedforward term to help filter move with fast changes in input*/
    if ((ADCSample > (filteredValue + FILTER_FEEDFORWARD)) || 
        (ADCSample < (filteredValue - FILTER_FEEDFORWARD)))
	{
		filteredValue = ADCSample;
	}
	else
	{
	    filteredValue = filteredValue + ((ADCSample - filteredValue) >> TC_FILTER_COEFF_SHIFT);
	}
	
	/* Compensate filter result for  left shift of 8 and round off */
	filValueRounded = (filteredValue >> MAX_FILTER_COEFF_SHIFT) + 
    ((filteredValue & 0x00000080) >> (MAX_FILTER_COEFF_SHIFT - 1));
	
	return filValueRounded;
}  

/* [] END OF FILE */
