/*******************************************************************************
* File Name: main.c
*
* Version: 1.20
*
* Description:
*  This is source code for the datasheet example of the Thermistor Calculator
*  component.
*
********************************************************************************
* Copyright 2012-2014, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

/* Defines for AMux channels */
#define THERM_REF_CHANNEL       (1u)
#define THERM_SIGNAL_CHANNEL    (0u)

/* More samples means smoother data but values may lag
   powers of 2 make the math easier for the CPU and should be used. */
#define SAMPLES                 (256u)

/* Degree sign */ 
#define DEGREE                  (0xDFu)


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function of the Thermistor Calculator datasheet example project. 
*  For a detailed description please refer to the project schematic.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    int32 temperature = 0;
    int32 absTemp;
    int16 vRef, vTherm;    

    /* used to denote the first sampling time */
    uint8 init = 0u;
    
    /* Start ADC, AMux, OpAmp components */
	ADC_DelSig_Start();
	AMux_Start();
	Opamp_Start();
    
    /* Start LCD and display static texts */
    LCD_Start();
	LCD_Position(0u, 0u);
	LCD_PrintString("Thermistor Example");
    LCD_Position(1u, 0u);
	LCD_PrintString("T =     .   C");
    LCD_Position(1u, 11u);
    LCD_PutChar(DEGREE);
    
    for(;;)
    {
        /* Read voltage across thermistor */
        AMux_FastSelect(THERM_SIGNAL_CHANNEL);
        ADC_DelSig_StartConvert();
        ADC_DelSig_IsEndConversion(ADC_DelSig_WAIT_FOR_RESULT);
        vTherm = ADC_DelSig_GetResult32();
    	
        /* Read voltage across reference resistor */
        AMux_FastSelect(THERM_REF_CHANNEL);
        ADC_DelSig_StartConvert();
        ADC_DelSig_IsEndConversion(ADC_DelSig_WAIT_FOR_RESULT);
        vRef = ADC_DelSig_GetResult32();
    
        /* Find temperature in Celsius degree using the component API */
        temperature += Thermistor_GetTemperature(Thermistor_GetResistance(vRef, vTherm));

        /* If we are initialized */
        if(init)
        {
            /* Averaging over a number of samples, gives a much smoother look to the numbers */
            temperature -= (temperature/SAMPLES);
        }else{
            /* First time through the loop, no data, so replicate the first value */
            temperature *= SAMPLES;
        }
        init = 1;
        
        absTemp = temperature;
        
        LCD_Position(1u, 8u);
        LCD_PutChar('.');
        
        LCD_Position(1u, 11u);
        LCD_PutChar(DEGREE);
        
            absTemp /= SAMPLES;
            LCD_PutChar('C');

            LCD_PrintString("        ");
        
        if (temperature < 0)
        {        
            LCD_Position(1u, 4u);
            LCD_PrintString("-");
            absTemp *= -1;
        }
        else
        {
            LCD_Position(1u, 4u);
            LCD_PrintString("+");
        }
        LCD_Position(1u, 5u);
        
        /* if less than 100, put a space */
        if(absTemp < 10000)
            LCD_PutChar(' ');
            
        /* if less than 10 put a space */
        if(absTemp <= 1000)
            LCD_PutChar(' ');
        
        LCD_PrintDecUint16(absTemp / 100u);
        LCD_Position(1u, 9u);
        if(absTemp % 100u < 10)
        {
            /* if less than 10, put a 0 after the '.' */
            LCD_PrintDecUint16(0);
        }
        
        LCD_PrintDecUint16(absTemp % 100u);
    }
}

/* [] END OF FILE */
