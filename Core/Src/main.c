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
#include <string.h>
#include <stdio.h>
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
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void spi_tx(uint8_t);
void clear_screen();
void send_char(char);
void send_str(char *str, int size);
void send_hex(int);
void reset_lcd();
void set_dc_state(int stat);
void init_nokia5110();
void print_str(char *str, int str_size);

void aht20_init_parms();
int check_calibration_enable_bit();
int send_init_command();
int send_measurment_command();
int read_hum_temp_data(float *hum, float *temp);
int read_measurment_status();
int init_aht20();
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
#define D 0x1f0a04
#define E 0x1f1515
#define F 0x1f0505
#define G 0x1f1119
#define H 0x1f041f
#define I 0x111f11
#define J 0x000000
#define K 0x000000
#define L 0x000000
#define M 0x1f061f
#define N 0x1f0c1f
#define O 0x000000
#define P 0x1f0507
#define Q 0x000000
#define R 0x000000
#define S 0x17151d
#define T 0x011f01
#define U 0x1f101f

#define ZERO 	0x1f111f
#define ONE 	0x111f10
#define TWO 	0x1d1517
#define THREE 0x15151f
#define FOUR 	0x07041f
#define FIVE 	0x17151d
#define SIX 	0x1f151d
#define SEVEN 0x01011f
#define EIGHT 0x1f151f
#define NINE 	0x17151f

#define DOT 	0x100000
#define COLON 0x001100

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
	float hum, temp;
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

	// initiate nokia5110 pin configurations
	port_pin dc = {.port = GPIOB, .pin = LL_GPIO_PIN_11};
	port_pin rst = {.port = GPIOB, .pin = LL_GPIO_PIN_10};
	port_pin ce = {.port = GPIOB, .pin = LL_GPIO_PIN_1};
	nokia5110.dc = dc;
	nokia5110.ce = ce;
	nokia5110.rst = rst;
	//

	init_nokia5110();
	
	rc = init_aht20();
	if(-1 == rc)
	{
		return -1;
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	char str[50] = {0};
  while (1)
  {
		rc = read_hum_temp_data(&hum, &temp);
		if(-1 == rc)
		{
			return -1;
		}
		
		sprintf(str, "hum is:%.1f and temp is:%.1f", 
		hum,temp
	);
	print_str(str, strlen(str));
		
	LL_mDelay(4000);
    


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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1|LL_GPIO_PIN_10|LL_GPIO_PIN_11);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1|LL_GPIO_PIN_10|LL_GPIO_PIN_11;
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
		while(!LL_SPI_IsActiveFlag_TXE(SPI1));
		LL_SPI_TransmitData8(SPI1, data);
		if(LL_SPI_IsActiveFlag_RXNE(SPI1))
		{}
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
		
		case 'a':
			send_hex(A);
		break;

		case 'B':
			send_hex(B);
		break;

		case 'b':
			send_hex(B);
		break;

		case 'C':
			send_hex(C);
		break;
		
		case 'c':
			send_hex(C);
		break;

		case 'D':
			send_hex(D);
		break;

		case 'd':
			send_hex(D);
		break;

		case 'E':
			send_hex(E);
		break;

		case 'e':
			send_hex(E);
		break;

		case 'F':
			send_hex(F);
		break;
		
		case 'f':
			send_hex(F);
		break;

		case 'G':
			send_hex(G);
		break;
		
		case 'g':
			send_hex(G);
		break;

		case 'H':
			send_hex(H);
		break;
		
		case 'h':
			send_hex(H);
		break;
		
		case 'I':
			send_hex(I);
		break;
		
		case 'i':
			send_hex(I);
		break;

		case 'J':
			
		break;
		case 'K':
			
		break;
		case 'L':
			
		break;
		
		case 'M':
			send_hex(M);
		break;
		
		case 'm':
			send_hex(M);
		break;
		
		
		case 'N':
			send_hex(N);
		break;

		case 'n':
			send_hex(N);
		break;
		
		case 'O':
			
		break;

		case 'P':
			send_hex(P);
		break;
		
		case 'p':
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
		
		case 's':
			send_hex(S);
		break;

		case 'T':
			send_hex(T);
		break;
		
		case 't':
			send_hex(T);
		break;
		
		case 'U':
			send_hex(U);
		break;

		case 'u':
			send_hex(U);
		break;

		
		case ' ':
			send_hex(SPACE);
		break;
		
		case '1':
			send_hex(ONE);
		break;
		
		case '2':
			send_hex(TWO);
		break;
		
		case '3':
			send_hex(THREE);
		break;
		
		case '4':
			send_hex(FOUR);
		break;
		
		case '5':
			send_hex(FIVE);
		break;
		
		case '6':
			send_hex(SIX);
		break;
		
		case '7':
			send_hex(SEVEN);
		break;
		
		case '8':
			send_hex(EIGHT);
		break;
		
		case '9':
			send_hex(NINE);
		break;

		case '0':
			send_hex(ZERO);
		break;
		
		case '.':
			send_hex(DOT);
		break;
		
		case ':':
			send_hex(COLON);
		break;
		
		default:
			send_hex(0xffffff);
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

void init_nokia5110()
{
	reset_lcd();

	set_dc_state(STATE_OFF); // DC to 0

	// set SCE to high to initialize serial protocol
	LL_GPIO_SetOutputPin(nokia5110.ce.port, nokia5110.ce.pin);
	LL_mDelay(10);

	LL_SPI_Enable(SPI1);

	// set CE to low
	LL_GPIO_ResetOutputPin(nokia5110.ce.port, nokia5110.ce.pin); // SCE to 0
	spi_tx(0x21); // function set PD=0 and V=0, select extended instruction set (H=1)
	spi_tx(0xff); // setting voltage
	spi_tx(0x20); // return to normal instruction set, PD=0, V=0, H=0
	spi_tx(0x0c); // display control set normal mode, D=1 and E=0

}

void print_str(char *str, int str_size)
{
	uint8_t init_x = 0x80;
	set_dc_state(STATE_ON); // DC to 1
	clear_screen();
	set_dc_state(STATE_OFF); // DC to 0
	spi_tx(init_x);
	spi_tx(0x40);
	set_dc_state(STATE_ON);// DC to 1
	send_str(str, str_size);
  set_dc_state(STATE_OFF);
}



//////////////
///////////////
/////////////
////////////////
int init_aht20()
{
	int rc;
	
	aht20_init_parms();
	
	LL_mDelay(100);
	
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




int read_hum_temp_data(float *hum, float *temp)
{
	int rc;
	uint8_t pData[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint32_t humData = 0;
	uint32_t tempData = 0;
	
	
	rc = send_measurment_command();
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
	
	rc = HAL_I2C_Master_Receive(&hi2c1, (aht20.dev_addr << 1) | 0x01, pData, sizeof(pData), I2C_TIMEOUT);
	if(HAL_OK != rc)
	{
		return -1;
	}
	
	humData = (humData | pData[1]) << 8;
	humData = (humData | pData[2]) << 8;
	humData = (humData | pData[3]);
	humData = humData >> 4;	
	*hum = (float)(humData*100*10/1024/1024) / 10;
	
	tempData = (tempData | pData[3]) << 8;
	tempData = (tempData | pData[4]) << 8;
	tempData = (tempData | pData[5]);
	tempData = tempData&0x0fffff;
	*temp = (float)(tempData*200*10/1024/1024-500) / 10;
	
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
