/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "GEVA.h"
#include "stdio.h"
#include "stm32_ssd1306_i2c.h"
#include "stm32_max6675.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CHAR_BUFF_SIZE 6
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//float to string function
char * float_to_char(float x, char *p) {
    char *s = p + CHAR_BUFF_SIZE; // go to end of buffer
    uint16_t decimals;  // variable to store the decimals
    int units;  // variable to store the units (part to left of decimal place)
    if (x < 0) { // take care of negative numbers
        decimals = (int)(x * -100) % 100; // make 1000 for 3 decimals etc.
        units = (int)(-1 * x);
    } else { // positive numbers
        decimals = (int)(x * 100) % 100;
        units = (int)x;
    }

    *--s = (decimals % 10) + '0';
    decimals /= 10; // repeat for as many decimal places as you need
    *--s = (decimals % 10) + '0';
    *--s = '.';

    while (units > 0) {
        *--s = (units % 10) + '0';
        units /= 10;
    }
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    return s;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  //-------vbc_file start-------

vbc_file mamma_mia;

mamma_mia.header.id = 0xa7;
mamma_mia.header.width = 0x7f;
mamma_mia.header.height = 0x3f;
mamma_mia.header.byte_cnt_h = 0x03;
mamma_mia.header.byte_cnt_l = 0xf5;

uint8_t mamma_mia_sv[1008] = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x10, 0x08, 0x00, 0x06, 0x01, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x10, 0x08, 0x04, 0x03, 0x83, 0x00, 
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x10, 0x08, 0x06, 0x00, 0x02, 0x00, 
	0x10, 0x18, 0x30, 0x00, 0x6c, 0x20, 0x00, 0x00, 0x01, 0x02, 0x10, 0x08, 0x03, 0x00, 0x0e, 0x00, 
	0x18, 0x28, 0x30, 0x38, 0xee, 0x60, 0x00, 0x00, 0x01, 0x82, 0x10, 0x0c, 0x01, 0xc1, 0xf8, 0x00, 
	0x1c, 0x68, 0x78, 0x6c, 0xaa, 0xe3, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
	0x16, 0x48, 0x48, 0x45, 0xab, 0xa7, 0x80, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 
	0x13, 0xc8, 0x48, 0x47, 0x29, 0xa4, 0x80, 0x00, 0x00, 0x63, 0x87, 0x7b, 0xff, 0xe7, 0x00, 0x00, 
	0x10, 0x08, 0x8c, 0x46, 0x29, 0x24, 0x80, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
	0x10, 0x08, 0x84, 0x46, 0x28, 0x24, 0xc0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
	0x10, 0x08, 0x84, 0x40, 0x28, 0x24, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
	0x10, 0x08, 0x84, 0x40, 0x28, 0x27, 0xf0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
	0x10, 0x09, 0xfe, 0x40, 0x28, 0x24, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
	0x10, 0x09, 0x02, 0x40, 0x28, 0x2c, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
	0x10, 0x09, 0x02, 0x40, 0x28, 0x28, 0x40, 0x00, 0x04, 0x10, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
	0x10, 0x09, 0x03, 0x40, 0x28, 0x28, 0x00, 0x00, 0x04, 0x18, 0x00, 0x1f, 0xff, 0xe4, 0x00, 0x00, 
	0x10, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x04, 0x0f, 0xff, 0xf0, 0x00, 0x3e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x11, 0xc4, 0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0x11, 0x44, 0x10, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x11, 0x64, 0x10, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x21, 0x13, 0x24, 0x10, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1c, 0x40, 0x21, 0x12, 0x24, 0x30, 0x1f, 0x00, 0x1f, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x30, 0x40, 0x21, 0x12, 0x20, 0x20, 0x7f, 0x80, 0x1f, 0x80, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x40, 0x21, 0x13, 0xe0, 0x20, 0x7f, 0xc0, 0x3f, 0xc0, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0xc0, 0x21, 0x16, 0x20, 0x20, 0x7f, 0x40, 0x3f, 0x40, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 0x00, 0x04, 0x24, 0x20, 0x7e, 0x40, 0x7e, 0x60, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x3c, 0x61, 0x80, 0x00, 0x00, 0x24, 0x20, 0x3e, 0x60, 0x1f, 0xc0, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x64, 0x41, 0x00, 0x00, 0x00, 0x00, 0x20, 0x03, 0xe0, 0x00, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x44, 0x43, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0xc4, 0x42, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x10, 0x00, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x84, 0x42, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x10, 0x80, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x84, 0x42, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x30, 0x00, 0x00, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x84, 0x42, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x01, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x84, 0x42, 0x00, 0x00, 0x00, 0x00, 0x20, 0x7c, 0x20, 0x7f, 0xfd, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x84, 0x42, 0x00, 0x00, 0x00, 0x00, 0x21, 0xff, 0xf0, 0x7f, 0xff, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc4, 0xc2, 0x03, 0xf8, 0x00, 0x00, 0x27, 0xff, 0xf8, 0xff, 0xff, 0x0f, 0x80, 
	0x00, 0x00, 0x00, 0x46, 0x82, 0x0e, 0x0c, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0xf8, 0x1f, 0x0f, 0xc0, 
	0x00, 0x00, 0x00, 0x42, 0x82, 0x18, 0x06, 0x01, 0xc0, 0x1f, 0xff, 0xff, 0xf0, 0x0f, 0x8c, 0xe0, 
	0x00, 0x00, 0x01, 0xe3, 0x83, 0x30, 0x02, 0x07, 0xe0, 0x3c, 0x00, 0x3f, 0xe0, 0x07, 0x80, 0x60, 
	0x00, 0x00, 0x03, 0x61, 0xc1, 0xe0, 0x03, 0x0e, 0x70, 0xb8, 0x00, 0x1f, 0xe7, 0x83, 0xc0, 0x60, 
	0x00, 0x00, 0x02, 0x71, 0xc0, 0xc3, 0xe0, 0xcc, 0x30, 0xfc, 0x00, 0x16, 0xfc, 0x83, 0xc0, 0x60, 
	0x00, 0x00, 0x02, 0x50, 0xe0, 0x06, 0x30, 0x4c, 0x01, 0xe6, 0x01, 0xc1, 0xc1, 0x82, 0xe0, 0xe0, 
	0x00, 0x00, 0x02, 0x78, 0x60, 0x04, 0x18, 0x4c, 0x03, 0xe2, 0x00, 0xfe, 0x03, 0x04, 0x7f, 0xe0, 
	0x00, 0x00, 0x02, 0x2c, 0x00, 0x0c, 0x0f, 0xce, 0x03, 0xc3, 0x00, 0x60, 0x0e, 0x0c, 0x3f, 0xc0, 
	0x00, 0x00, 0x03, 0x36, 0x00, 0x08, 0x19, 0x87, 0x0f, 0xc1, 0x80, 0x38, 0x38, 0x08, 0x3f, 0x80, 
	0x00, 0x00, 0x01, 0x1f, 0x00, 0x00, 0x18, 0x87, 0xff, 0x80, 0x80, 0x0f, 0xe0, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x30, 0x83, 0xff, 0x00, 0xc0, 0x00, 0x00, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x1f, 0xe0, 0x81, 0xfc, 0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x38, 0x00, 0x01, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x06, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x03, 0x30, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x10, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x61, 0xe0, 0x1e, 0x10, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0c, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0xe0, 0x3f, 0xf0, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0x0c, 0x00, 0x30, 0x00, 0x00, 0x07, 0xb0, 0x07, 0x00, 0x3c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x86, 0x00, 0xe0, 0x00, 0x00, 0x1d, 0x18, 0x07, 0x80, 0x66, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc3, 0x8f, 0x80, 0x00, 0x00, 0xf0, 0x0e, 0x0f, 0xe1, 0xc3, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x03, 0x81, 0x03, 0xc8, 0x37, 0x01, 0x80, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x04, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x78, 0xfc, 0x00, 0xc0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x58, 0x04, 0x00, 0x00, 0x08, 0x00, 0x00, 0x01, 0x80, 0x00, 0x60, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x4f, 0xdc, 0x00, 0x00, 0x38, 0x00, 0x00, 0x03, 0x00, 0x00, 0x20, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x30, 0x00
};

mamma_mia.body = mamma_mia_sv;

//-------vbc_file stop -------


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_ADC_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  video_buffer vb;  //lcd video buffer
  init_buf(&vb,128,64);
  init_display(0x78,&hi2c1);  //addr = 0x3c<<1

  
  set_mode(INVERTED,&vb);
  //put_pixel(10,10,SET_C,&vb);
  //put_circle(20,20,10,&vb);
  

  max6675 max6675A;
  max6675 max6675B;
  max6675_init(&max6675A,CS_A_GPIO_Port,CS_A_Pin,&hspi1);
  max6675_init(&max6675B,CS_B_GPIO_Port,CS_B_Pin,&hspi1);
  
  char temp_str[] = {32,32,32,32,32,32};
  //char temp_str[] = {0,0,0,0,0,0};
 
  put_vbc(mamma_mia,0,1,&vb);
  update_display(&vb);
  
  HAL_Delay(1500);    



  //put_string(0,0,temp_str,SMALL,&vb);
  //put_string(0,40,"temp_str",SMALL,&vb);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 char c[CHAR_BUFF_SIZE];
  

  while (1)
  {

 
    //clear_sector(0,0,60,15,&vb);
    //max6675_read_temp(max6675B);
    //put_string(0,0,"MAMMA MIA",MEDIUM,&vb);
    HAL_UART_Transmit(&huart1,float_to_char(max6675_read_temp(max6675B),temp_str),CHAR_BUFF_SIZE,100);
    HAL_UART_Transmit(&huart1,"\n\r",2,100);
    HAL_GPIO_TogglePin(SSR_A_GPIO_Port,SSR_A_Pin);
    //float_to_char(max6675_read_temp(max6675A),temp_str);
    /*for(uint8_t i=0; i< CHAR_BUFF_SIZE; i++)
      put_char(i*10,0,temp_str[i],MEDIUM,&vb);
*/
    //put_char(0,0,'K',MEDIUM,&vb);
    
    //put_circle(10,10,5,&vb);
    
    //update_display(&vb);
    HAL_Delay(250);    
  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_9;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000102;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /** I2C Fast mode Plus enable
  */
  __HAL_SYSCFG_FASTMODEPLUS_ENABLE(I2C_FASTMODEPLUS_I2C1);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 0;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RE_485_Pin|DE_485_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|GPIO_PIN_14|SSR_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_B_Pin|CS_A_Pin|SSR_A_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ZeroCrossing_Pin */
  GPIO_InitStruct.Pin = ZeroCrossing_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ZeroCrossing_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RE_485_Pin DE_485_Pin */
  GPIO_InitStruct.Pin = RE_485_Pin|DE_485_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : OK_BTN_Pin RT_BTN_Pin LT_BTN_Pin DWN_BTN_Pin */
  GPIO_InitStruct.Pin = OK_BTN_Pin|RT_BTN_Pin|LT_BTN_Pin|DWN_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin PB14 SSR_B_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|GPIO_PIN_14|SSR_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_B_Pin CS_A_Pin SSR_A_Pin */
  GPIO_InitStruct.Pin = CS_B_Pin|CS_A_Pin|SSR_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : UP_BTN_Pin */
  GPIO_InitStruct.Pin = UP_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(UP_BTN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
