/******************************************************************************
* Project Name		:      CE219905_PSoC3_5_Thermocouple_Low_End
* File Name			:      main.c
* Version			:      1.0
* Device Used		:      CY8C3866AXI-040, CY8C5868AXI-LP035
* Software Used		:      PSoC Creator 4.2 
* Compiler Used		:      Keil(C51),ARM GCC
* Related Hardware	:	   CY8CKIT-030 PSoC 3 DVK, 
						   CY8CKIT-050 PSoC 5 DVK, 
						   CY8CKIT-025 PSoC Precision Analog Temperature EBK
********************************************************************************
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
********************************************************************************

*******************************************************************************
*                           THEORY OF OPERATION
* This project showcases thermocouple temperature measurement using PSoC3/5. 
* The ADC senses the thermo emf and the output voltage of the cold junction
* temperature sensor IC. The IC voltage is converted to temperature and the 
* cold junction compensation voltage corresponding to the cold junction 
* temperature is computed. This voltage is added to the hot junction voltage 
* and the obtained voltage is converted back to temperature. 

* The thermocouple component helps in voltage to temperature conversion and 
* vice versa. AN75511 provides the complete theory behind thermocouple 
* temperature measurement.
*******************************************************************************/
#include <main.h>

/* Variable that selects cold junction temperature sensor. Gets updated in ISR */
uint8 volatile CJSelectFlag = 0;			  

/*Button Press Interrupt Handler*/
CY_ISR(switchISRhandler)
{
    CJSelectFlag ^= 0x01;
}

int main(void)
{
    /* VSSA reference voltage */
    int16 offset;
    
	/* TC cold junction temperature */
    int16 coldJnTemp;
		
	/* cold junction voltage calculated from the cold junction temperature */
    int32 tcColdJnuVolt;
     
    /* TC hot junction ADC count measurement */
    int32 hotJnCount;
    
    /* Filtered hot junction count */
    int32 filHotJnCount;
	
    /* Thermocouple volt calculated from ADC hot Jn count */
    int32 tcHotJnuVolt;
    
    /* Thermocouple voltage measured from tcCount 
     * (cold junction voltage + hot junction voltage) */
    int32 tcuVolt;
    
	/* TC hot junction temperature */
    int32 tcTemp;
	
	/* LCD display string */
	char displayStr[16] = {'\0'};
	
	/* Flag to indicate whether the program displays a new temperature first time after a Broken TC is detected */
	uint8 first = 0;
  
  	/* Initialize the blocks of the project */
	/* Start LCD and display static texts */
	LCD_Start();
	LCD_Position(0,0);
	LCD_PrintString("TC ");
	LCD_Position(1,0);
	LCD_PrintString("CJ ");
	LCD_Position(0,9);
	LCD_PrintString("CJ->SW2");
	
	/* Start ADC, MUX, OpAmp, enable interrupts */
	ADC_Start();
	ADCMux_Start();
	Opamp_Start();
    isr_CJChange_StartEx(switchISRhandler);
	CyGlobalIntEnable;
	
    for(;;)
    {
		/* Measure cold junction temperature */
	    coldJnTemp = MeasureColdJnSensorTemp();
	    
	    /* Convert cold junction temp to cold junction compensation voltage */
	    tcColdJnuVolt = Thermocouple_1_GetVoltage(coldJnTemp);
	    
	    /* Read thermocouple hot junction voltage */
	    ADCMux_FastSelect(TC_HOT_JN_CHANNEL);
		ADC_SelectConfiguration(ADC_TC_Config,1);
        ADC_StartConvert();
        /*Note in this example a blocking call is used, this can be replaced
        with a polling method is required in your application*/
	    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
	    ADC_StopConvert();
	    hotJnCount = ADC_GetResult16();
		
		/* Read ground reference voltage */
	    offset = MeasureOffset();
		
		/* Correlated double sampling to remove ADC offset */
	    hotJnCount -= offset;
	    
	    /* Filter signal */
	    filHotJnCount = FilterSignal(hotJnCount);
		
		/* Find thermo-emf in microvolts */
	    tcHotJnuVolt = ADC_CountsTo_uVolts(filHotJnCount);
		
		/* Perform Cold junction compensation */
	    tcuVolt = tcColdJnuVolt + tcHotJnuVolt;
		
		/* Convert thermo-emf to temperature */
	    tcTemp = Thermocouple_1_GetTemperature(tcuVolt);
	    
		/* If thermocouple is broken, display broken alert, else display thermocouple temperature */
		if (tcHotJnuVolt < BROKEN_TC_THRESHOLD)
		{
			LCD_Position(0,3);
			LCD_PrintString("Broken");
			first = 1;
		}
	    else
		{
			if (first)
			{
			    LCD_Position(0, 3);
				LCD_PrintString("      ");
				first = 0;
			}
			/* 50 is added to round-off the temperature so that the maximum error will be <=0.5 */		
			sprintf(displayStr, "%ld  ", ((tcTemp+50)/100));		
		    LCD_Position(0, 3);
			LCD_PrintString(displayStr); 
		}
		
		/* Display cold junction temperature */
		/* 50 is added to round-off the temperature so that the maximum error will be <=0.5 */
		sprintf(displayStr, "%d   ", ((coldJnTemp+50)/100));
	    LCD_Position(1, 3);
		LCD_PrintString(displayStr);
		
		/* Display which sensor was used for CJC */
        /* This is controlled by SW2*/
		if (CJSelectFlag == 0)
		{
			LCD_Position(1, 9);
			LCD_PrintString("IC");
		}
		else
		{
			LCD_Position(1, 9);
			LCD_PrintString("Th");
		}	
    }
}

/* [] END OF FILE */
