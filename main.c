/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void spi_tx(uint8_t);
void clear_screen();
void send_char(char);
void send_str(char *str, int size);
void send_hex(int);
void reset_lcd();
void set_dc_state(int stat);
void set_init_configs();
void print_str(char *str);

void aht20_init_parms();
int check_calibration_enable_bit();
int send_init_command();
int send_measurment_command();
int read_hum_temp_data(int *hum, int *temp);
int read_measurment_status();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define I2C_TIMEOUT 10000

typedef struct port_pin
{
	int pin;
	GPIO_TypeDef *port;
} port_pin;

typedef struct nokia5110_pin_mapping
{
	port_pin dc;
	port_pin ce;
	port_pin rst;
	port_pin bl;
} nokia5110_pin_mapping;

typedef struct aht20_parm
{
	uint8_t dev_addr;
	
	uint8_t init_command;
	uint8_t init_command_parms[2];
	
	uint8_t measurment_command[3];
	
	uint8_t get_stat_word_command;
} aht20_parm;

nokia5110_pin_mapping nokia5110;
aht20_parm aht20;

#define A 0x1f051f
#define B 0x1f150e
#define C 0x0e0a11
#define D 0x000000
#define E 0x1f1515
#define F 0x1f0505
#define G 0x1f1119
#define H 0x1f041f
#define I 0x111f11
#define J 0x000000
#define K 0x000000
#define L 0x000000
#define M 0x000000
#define N 0x1f0c1f
#define O 0x000000
#define P 0x1f0507
#define Q 0x000000
#define R 0x000000
#define S 0x000000
#define T 0x011f01


#define SPACE 0x000000
#define STATE_ON  1
#define STATE_OFF 0

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char str[12] = "HI CAATAINHB";
	int hum, temp;
	int rc;
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
  /* USER CODE BEGIN 2 */
	
	// initiate nokia5110 pin configurations
	port_pin dc = {.port = GPIOA, .pin = LL_GPIO_PIN_2};
	port_pin rst = {.port = GPIOA, .pin = LL_GPIO_PIN_0};
	port_pin ce = {.port = GPIOA, .pin = LL_GPIO_PIN_1};
	nokia5110.dc = dc;
	nokia5110.ce = ce;
	nokia5110.rst = rst;
	//
	
	set_init_configs();

	aht20_init_parms();
	LL_mDelay(100);

  LL_mDelay(40);
	rc = check_calibration_enable_bit();
	if(0 == rc)
	{
		if(-1 == send_init_command())
		{
			return -1;
		}
		LL_mDelay(10);
	}
	else if(-1 == rc)
	{
		return -1;
	}
	

	rc = send_measurment_command();
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
	if(-1 == rc)
	{
		return -1;
	}
	LL_mDelay(80);
	
	rc = read_measurment_status();
	if(-1 == rc || 0 == rc)
	{
		return -1;
	}

	read_hum_temp_data(&hum, &temp);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
  while (1)
  {
		/*
		print_str(str);
		LL_mDelay(1000);
    */
		
		
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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_SetSystemCoreClock(8000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void set_dc_state(int stat)
{
	LL_mDelay(10);
	if(0 == stat)
	{
		LL_GPIO_ResetOutputPin(nokia5110.dc.port, nokia5110.dc.pin); 
	}
	else if(1 == stat)
	{
		LL_GPIO_SetOutputPin(nokia5110.dc.port, nokia5110.dc.pin); 
	}
}

void reset_lcd()
{
	LL_GPIO_SetOutputPin(nokia5110.rst.port, nokia5110.rst.pin);
	LL_mDelay(10);
	LL_GPIO_ResetOutputPin(nokia5110.rst.port, nokia5110.rst.pin);
	LL_mDelay(10);
	LL_GPIO_SetOutputPin(nokia5110.rst.port, nokia5110.rst.pin);
}

void spi_tx(uint8_t data)
{
		/*while(!LL_SPI_IsActiveFlag_TXE(SPI1));
		LL_SPI_TransmitData8(SPI1, data);
		if(LL_SPI_IsActiveFlag_RXNE(SPI1))
		{}*/
}

void clear_screen()
{
	for(int y = 0; y < 6; y++)
	{
		for(int i = 0; i < 84; i++)
		{
			spi_tx(0x00);
		}
	}
	
}

void send_char(char alph)
{
	switch(alph)
	{
		case 'A':
			send_hex(A);
		break;
		
		case 'B':
			send_hex(B);
		break;
		
		case 'C':
			send_hex(C);
		break;
		
		case 'D':
			send_hex(D);
		break;
		
		case 'E':
			send_hex(E);
		break;
		
		case 'F':
			send_hex(F);
		break;
		
		case 'G':
			send_hex(G);
		break;
		
		case 'H':
			send_hex(H);
		break;
		
		case 'I':
			send_hex(I);
		break;
		
		case 'J':
			spi_tx(J);
			spi_tx(J >> 8);
			spi_tx(J >> 16);
		break;
		case 'K':
			spi_tx(K);
			spi_tx(K >> 8);
			spi_tx(K >> 16);
		break;
		case 'L':
			spi_tx(L);
			spi_tx(L >> 8);
			spi_tx(L >> 16);
		break;
		case 'M':
			spi_tx(M);
			spi_tx(M >> 8);
			spi_tx(M >> 16);
		break;
		case 'N':
			send_hex(N);
		break;
		
		case 'O':
			spi_tx(O);
			spi_tx(O >> 8);
			spi_tx(O >> 16);
		break;
		
		case 'P':
			send_hex(P);
		break;
		
		case 'Q':
			send_hex(Q);
		break;
		
		case 'R':
			send_hex(R);
		break;
		
		case 'S':
			send_hex(S);
		break;
		
		case 'T':
			send_hex(T);
		break;
		
		case ' ':
			send_hex(SPACE);
		break;
		
		
		default:
			break;
	}
	
	spi_tx(0x00);
    	
}


void send_hex(int hex)
{
			spi_tx(hex >> 16);
			spi_tx(hex >> 8);
			spi_tx(hex);
}

void send_str(char *str, int size)
{
	for(int i = 0; i < size; i++)
	{
		send_char(str[i]);
	}
}

void set_init_configs()
{
	reset_lcd();
	
	set_dc_state(STATE_OFF); // DC to 0
	
	// set SCE to high to initialize serial protocol
	LL_GPIO_SetOutputPin(nokia5110.ce.port, nokia5110.ce.pin);
	LL_mDelay(10);
	
	//LL_SPI_Enable(SPI1);
		
	// set CE to low
	LL_GPIO_ResetOutputPin(nokia5110.ce.port, nokia5110.ce.pin); // SCE to 0
	spi_tx(0x21); // function set PD=0 and V=0, select extended instruction set (H=1)
	spi_tx(0xff); // setting voltage
	spi_tx(0x20); // return to normal instruction set, PD=0, V=0, H=0
	spi_tx(0x0c); // display control set normal mode, D=1 and E=0
	
}

void print_str(char *str)
{
	uint8_t init_x = 0x80;
	set_dc_state(STATE_ON); // DC to 1
	clear_screen();
	set_dc_state(STATE_OFF); // DC to 0
	spi_tx(init_x); 
	spi_tx(0x40);
	set_dc_state(STATE_ON);// DC to 1
	send_str(str, 12);
  set_dc_state(STATE_OFF);
}






void aht20_init_parms()
{
	aht20.dev_addr = 0x38;
	
	aht20.init_command = 0xbe;
	aht20.init_command_parms[0] = 0x08;
	aht20.init_command_parms[1] = 0x00;
	
	aht20.measurment_command[0] = 0xac;
	aht20.measurment_command[1] = 0x33;
	aht20.measurment_command[2] = 0x00;
	
	aht20.get_stat_word_command = 0x71;
}



int check_calibration_enable_bit()
{
	uint8_t stat_word = 0x00;
	int rc;
	
	rc = HAL_I2C_Master_Transmit(&hi2c1, (aht20.dev_addr << 1) , &aht20.get_stat_word_command,
													sizeof(aht20.get_stat_word_command), I2C_TIMEOUT);  
	if(HAL_OK != rc)
	{
		return -1;
	}
	
	rc = HAL_I2C_Master_Receive(&hi2c1, (aht20.dev_addr << 1) | 0x01, &stat_word, sizeof(stat_word), I2C_TIMEOUT);
	if(HAL_OK != rc)
		return -1;
	
	if(0x01 == ( (stat_word >> 3) & 0x01 )) // 1 : calibrated, 0 : uncalibrated
	{
		return 1;
	}
	
	return 0;
	
}





int send_init_command()
{
	int rc;
	
	rc = HAL_I2C_Master_Transmit(&hi2c1, (aht20.dev_addr << 1),&aht20.init_command, 
													sizeof(aht20.init_command), I2C_TIMEOUT);
	if(rc)
		return -1;
	
	
	rc = HAL_I2C_Master_Transmit(&hi2c1, (aht20.dev_addr << 1), aht20.init_command_parms, 
													sizeof(aht20.init_command_parms), I2C_TIMEOUT);

	if(rc)
		return -1;
	
	return 0;
}




int send_measurment_command()
{
	int rc;
	
	rc = HAL_I2C_Master_Transmit(&hi2c1, (aht20.dev_addr << 1), aht20.measurment_command,
													sizeof(aht20.measurment_command), I2C_TIMEOUT);
	if(HAL_OK != rc)	
	{
		return -1;
	}
		
	return 0;
}




int read_hum_temp_data(int *hum, int *temp)
{
	int rc;
	uint8_t pData[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	rc = HAL_I2C_Master_Receive(&hi2c1, (aht20.dev_addr << 1) | 0x01, pData, sizeof(pData), I2C_TIMEOUT);
	if(HAL_OK != rc)
	{
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
		return -1;
	}
	
	return 0;
}


int read_measurment_status()
{
	int rc;
	int counter = 0;
	uint8_t stat_word = 0x00;
	
	
	rc = HAL_I2C_Master_Transmit(&hi2c1, (aht20.dev_addr << 1) | 0x01, &aht20.get_stat_word_command,
													sizeof(aht20.get_stat_word_command), I2C_TIMEOUT);
	if(HAL_OK != rc)	
	{
		return -1;
	}
	
	rc = HAL_I2C_Master_Receive(&hi2c1, (aht20.dev_addr << 1) | 0x01, &stat_word, sizeof(stat_word), I2C_TIMEOUT);
	if(HAL_OK != rc)
	{
		return -1;
	}
		
	
	while((stat_word & 0x80) == 0x80) // 1 : calibrated, 0 : uncalibrated
	{
		counter++;
		rc = HAL_I2C_Master_Receive(&hi2c1, (aht20.dev_addr << 1) | 0x01, &stat_word, sizeof(stat_word), I2C_TIMEOUT);
		if(HAL_OK != rc)
		{
			return -1;
		}
		if(100 == counter)
			return 0;
	}

	return 1;
	
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
