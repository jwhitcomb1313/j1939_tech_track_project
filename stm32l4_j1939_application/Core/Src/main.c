/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 #include <stdio.h>
 #include <stdint.h>
 #include <string.h>
 #include "uart.h"
 #include "can_spi.h"
 #include "mcp2515.h"
 
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
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/*************** ****************/
char id_buf[20]; 
char data_buf[20];

/*************** ****************/


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
/*************** ****************/

/*************** ****************/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*************** ****************/
  
/*************** ****************/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /*************** ****************/
  
  /*************** ****************/
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /*************** ****************/
  
  /*************** ****************/
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /*************** ****************/

  /*************** ****************/
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /*************** ****************/
  canspi_Init(); 
  // MCP_test_loopback_init(); 
  // uart_serial_print(end, sizeof(end));
   
  // testRegisterWrite(); 
  // can_msg_t tx_message;  
  // can_ext_id_t tx_id; 
  // uint32_t uId = tx_id.id;

  // // Load ID
  // tx_id.frame.priority = 0; 
  // tx_id.frame.edp = 0;
  // tx_id.frame.dp = 1; 
  // tx_id.frame.pf = 0xFF; 
  // tx_id.frame.ps = 0x01; 
  // tx_id.frame.source_address = 0xFC; 
  // // Load Frame
  // tx_message.frame.canId = tx_id.id; 
  // tx_message.frame.dlc = 8;
  // tx_message.frame.data0 = 0xFF; 
  // tx_message.frame.data1 = 0x1; 
  // tx_message.frame.data2 = 0x2;
  // tx_message.frame.data3 = 0x3;
  // tx_message.frame.data4 = 0x5;
  // tx_message.frame.data5 = 0x5;
  // tx_message.frame.data6 = 0x6;
  // tx_message.frame.data7 = 0x7;

  // can_msg_t rx_message;  

  // char buf[30];
  // static uint8_t canMessage[13];
  // uint8_t readByte = 0; 
  // canspi_TransmitMessage(&tx_message); 
  /*************** ****************/  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /*************** ****************/
  while (1)
  { 
    MCP_test_loopback_function(); 
    // if(canspi_ReceiveMessage(&rx_message))
    // {
    //   // canspi_CanLoopTest(rx_message); 
    //   sprintf(buf, "****** PCAN ******\r\n\r\n"); 
    //   // uart_serial_print(buf, sizeof(buf));
    //   // memset(buf, '\0', sizeof(buf));
    // } 
    // canspi_TransmitMessage(&tx_message); 
    // canspi_ReceiveMessage(&rx_message);
    // HAL_Delay(1000); 
    // canMessage[0] = 0xC7;   // SIDH
    // canMessage[1] = 0xCA;   // SIDL
    // canMessage[2] = 0xBC;   // EID8
    // canMessage[3] = 0x33;   // EID0
    // canMessage[4] = 1;      // DLC
    
    // canMessage[5] = 0xFF;      // D0-D8
    // canMessage[6] = 1;
    // canMessage[7] = 2;
    // canMessage[8] = 3;
    // canMessage[9] = 4;
    // canMessage[10] = 5;
    // canMessage[11] = 6;
    // canMessage[12] = 7;

    // MCP2515_WriteByte(0x31, canMessage[0]);
    // readByte = MCP2515_ReadByte(0x31);
    // sprintf(buf, "SIDH = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // MCP2515_WriteByte(0x32, canMessage[1]);
    // readByte = MCP2515_ReadByte(0x32);
    // sprintf(buf, "SIDL = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // MCP2515_WriteByte(0x33, canMessage[2]);
    // readByte = MCP2515_ReadByte(0x33);
    // sprintf(buf, "EID8 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // MCP2515_WriteByte(0x34, canMessage[3]);
    // readByte = MCP2515_ReadByte(0x34);
    // sprintf(buf, "EID0 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // MCP2515_WriteByte(0x35, canMessage[4]);
    // readByte = MCP2515_ReadByte(0x35);
    // sprintf(buf, "DLC = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // MCP2515_WriteByte(0x36, canMessage[5]);
    // readByte = MCP2515_ReadByte(0x36);
    // sprintf(buf, "D0 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // MCP2515_WriteMultipleBytes(0x31, canMessage, 13);
    // MCP2515_RequestToSend(0x81);

    // canspi_printTxRxErrorReg();
    // mcp2515_write_baseaddr(0x31, canMessage, 13);
    // mcp2515_transmit_req(0);




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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
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
  huart2.Init.BaudRate = 9600;
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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SP1_CS_GPIO_Port, SP1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : SP1_CS_Pin */
  GPIO_InitStruct.Pin = SP1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SP1_CS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
