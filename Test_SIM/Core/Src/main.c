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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
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
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const char apn[] = "v-internet";                                 // Change this to your Provider details
const char server[] = "huy.assfa.net";                           // Change this to your domain
const int port = 443;
const char resource[] = "/insert.php"; // Endpoint của API trên server
const uint32_t timeOut = 20000;        // Thời gian chờ cho các lệnh AT (ms)
char content[80];                      // Nội dung dữ liệu POST
char ATcommand[80];                    // Chuỗi lệnh AT được xây dựng
uint8_t buffer[100] = {0};             // Bộ đệm nhận dữ liệu từ module SIM
uint32_t previousTick;                 // Biến lưu thời gian trước đó
uint16_t distance;                     // Khoảng cách hoặc dữ liệu cần gửi

// Hàm gửi lệnh AT đến module SIM và nhận phản hồi
void SIMTransmit(char *cmd)
{
  memset(buffer, 0, sizeof(buffer));                             // Xóa bộ đệm trước khi sử dụng
  HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000); // Gửi lệnh AT qua UART2
  HAL_UART_Receive(&huart2, buffer, 100, 1000);                  // Nhận phản hồi từ module SIM

  // Gửi phản hồi nhận được qua UART6 để debug
  HAL_UART_Transmit(&huart6, (uint8_t *)"Response: ", strlen("Response: "), 1000);
  HAL_UART_Transmit(&huart6, buffer, strlen((char *)buffer), 1000);
  HAL_UART_Transmit(&huart6, (uint8_t *)"\r\n", 2, 1000);
}

// Hàm kiểm tra cấu hình APN trên module SIM
void checkAPN(void)
{
  uint8_t APNisOK = 0;
  previousTick = HAL_GetTick(); // Ghi nhận thời gian bắt đầu kiểm tra

  while (!APNisOK && previousTick + timeOut > HAL_GetTick())
  {
    SIMTransmit("AT+CGDCONT?\r\n"); // Gửi lệnh kiểm tra cấu hình APN hiện tại

    if (strstr((char *)buffer, apn)) // Kiểm tra phản hồi chứa thông tin APN đúng
    {
      APNisOK = 1;
      HAL_UART_Transmit(&huart6, (uint8_t *)"APN duoc cau hinh thanh cong!.\r\n", strlen("APN duoc cau hinh thanh cong!.\r\n"), 1000);
    }
    else
    {
      HAL_UART_Transmit(&huart6, (uint8_t *)"APN cau hinh sai...\r\n", strlen("APN cau hinh sai...\r\n"), 1000);
      sprintf(ATcommand, "AT+CGDCONT=1,\"IP\",\"%s\",\"0.0.0.0\",0,0\r\n", apn); // Lệnh cấu hình lại APN
      SIMTransmit(ATcommand);
    }
    HAL_Delay(1000); // Đợi 1 giây trước khi thử lại
  }

  if (!APNisOK) // Nếu không thành công sau thời gian chờ
  {
    HAL_UART_Transmit(&huart6, (uint8_t *)"APN configuration failed.\r\n", strlen("APN configuration failed.\r\n"), 1000);
  }
}

void httpPost(void)
{
  // Cấu hình HTTP
  SIMTransmit("AT+HTTPINIT\r\n"); // Khởi tạo HTTP
  HAL_Delay(2000);                // Đợi một chút sau khi khởi tạo

  // Cấu hình URL (Server và Endpoint)
  sprintf(ATcommand, "AT+HTTPPARA=\"URL\",\"https://%s%s\"\r\n", server, resource);
  SIMTransmit(ATcommand); // Cấu hình URL của server
  HAL_Delay(2000);

  // Cấu hình loại nội dung (Content-Type)
  SIMTransmit("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n");
  HAL_Delay(2000);

  // Xác định kích thước dữ liệu POST
  sprintf(ATcommand, "AT+HTTPDATA=%d,10000\r\n", strlen(content)); // Thời gian chờ 10000ms
  SIMTransmit(ATcommand);
  HAL_Delay(2000);

  // Gửi dữ liệu POST
  SIMTransmit(content); // Nội dung sẽ được gửi dưới dạng body của POST
  HAL_Delay(2000);

  // Gửi lệnh POST đến server
  SIMTransmit("AT+HTTPACTION=1\r\n"); // 1 là HTTP POST
  HAL_Delay(5000);                    // Đợi phản hồi từ server

  // Kiểm tra mã trạng thái HTTP từ server (200 OK)
  if (strstr((char *)buffer, "+HTTPACTION: 1,200"))
  {
    HAL_UART_Transmit(&huart6, (uint8_t *)"POST success.\r\n", strlen("POST success.\r\n"), 1000);
  }
  else
  {
    HAL_UART_Transmit(&huart6, (uint8_t *)"POST failed.\r\n", strlen("POST failed.\r\n"), 1000);
  }

  // Đóng HTTP
  SIMTransmit("AT+HTTPTERM\r\n");
  HAL_Delay(2000);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  checkAPN(); // Kiểm tra APN trước khi sử dụng HTTP POST

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    distance = 100;
    sprintf(content, "distance=%d", distance);
    httpPost();
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

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
