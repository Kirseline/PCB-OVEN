#ifndef __MAX6675__
#define __MAX6675__

#include "stm32f0xx.h"

typedef struct max6675{
    GPIO_TypeDef *cs_pin_port;
    uint16_t cs_pin;
    SPI_HandleTypeDef *spi;
    uint8_t init_status;
}max6675;

void max6675_init(max6675 *chip, GPIO_TypeDef *cs_pin_port, uint16_t cs_pin, SPI_HandleTypeDef *spi);

 //return -9999 in case of initialization error
 //return 9999 in case of thermocouple disconnection
float max6675_read_temp(max6675 chip);

#endif