/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ZeroCrossing_Pin GPIO_PIN_13
#define ZeroCrossing_GPIO_Port GPIOC
#define RE_485_Pin GPIO_PIN_14
#define RE_485_GPIO_Port GPIOC
#define DE_485_Pin GPIO_PIN_15
#define DE_485_GPIO_Port GPIOC
#define VOLTAGE_A_Pin GPIO_PIN_0
#define VOLTAGE_A_GPIO_Port GPIOA
#define CURRENT_A_Pin GPIO_PIN_1
#define CURRENT_A_GPIO_Port GPIOA
#define VOLTAGE_B_Pin GPIO_PIN_4
#define VOLTAGE_B_GPIO_Port GPIOA
#define CURRENT_B_Pin GPIO_PIN_6
#define CURRENT_B_GPIO_Port GPIOA
#define NTC_A_Pin GPIO_PIN_0
#define NTC_A_GPIO_Port GPIOB
#define NTC_B_Pin GPIO_PIN_1
#define NTC_B_GPIO_Port GPIOB
#define OK_BTN_Pin GPIO_PIN_2
#define OK_BTN_GPIO_Port GPIOB
#define RT_BTN_Pin GPIO_PIN_10
#define RT_BTN_GPIO_Port GPIOB
#define LT_BTN_Pin GPIO_PIN_11
#define LT_BTN_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define CS_B_Pin GPIO_PIN_8
#define CS_B_GPIO_Port GPIOA
#define CS_A_Pin GPIO_PIN_11
#define CS_A_GPIO_Port GPIOA
#define UP_BTN_Pin GPIO_PIN_12
#define UP_BTN_GPIO_Port GPIOA
#define SSR_A_Pin GPIO_PIN_15
#define SSR_A_GPIO_Port GPIOA
#define SSR_B_Pin GPIO_PIN_3
#define SSR_B_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_8
#define FAN_GPIO_Port GPIOB
#define DWN_BTN_Pin GPIO_PIN_9
#define DWN_BTN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
