#include"stm32_max6675.h"

void max6675_init(max6675 *chip, GPIO_TypeDef *cs_pin_port, uint16_t cs_pin, SPI_HandleTypeDef *spi){

    chip->cs_pin_port = cs_pin_port;
    chip->cs_pin = cs_pin;
    chip -> spi = spi;
    
    HAL_GPIO_WritePin(chip->cs_pin_port, chip->cs_pin,GPIO_PIN_SET);

    chip-> init_status = 1;
};

float max6675_read_temp(max6675 chip){

if (!chip.init_status)
    return -9999;

float temp=0;
uint8_t TCF=0;                                          // Thermocouple Connection acknowledge Flag
uint8_t DATARX[2];
                                         // Temperature Variable
HAL_GPIO_WritePin(chip.cs_pin_port, chip.cs_pin,GPIO_PIN_RESET);       // Low State for SPI Communication
HAL_SPI_Receive(chip.spi,DATARX,1,50);                  // DATA Transfer
HAL_GPIO_WritePin(chip.cs_pin_port, chip.cs_pin,GPIO_PIN_SET);         // High State for SPI Communication
TCF=(((DATARX[0]|(DATARX[1]<<8))>>2)& 0x0001);        // State of Connecting

if(TCF)
    return 9999;

temp=((((DATARX[0]|DATARX[1]<<8)))>>3);               // Temperature Data Extraction
temp*=0.25;                                           // Data to Centigrade Conversation
//HAL_Delay(250);                                       // Waits for Chip Ready(according to Datasheet, the max time for conversion is 220ms)
return temp;
}