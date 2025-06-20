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
#include <time.h>
#include "SHT.h"
#include "bmp280.h"
#include <stdlib.h>
#define I2C_TIMEOUT_BUSY_FLAG 1000
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

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const char apn[] = "v-internet";                                 // Change this to your Provider details
const char server[] = "huyed2.assfa.net";                        // Change this to your domain
const int port = 443;
const char resource[] = "/DoAn2.php"; // Endpoint của API trên server
const uint32_t timeOut = 15000;        // Th�?i gian ch�? cho các lệnh AT (ms)
char content[256];                      // Nội dung dữ liệu POST
char ATcommand[80];                    // Chuỗi lệnh AT được xây dựng
uint8_t buffer[100] = {0};             // Bộ đệm nhận dữ liệu từ module SIM
uint32_t previousTick;                 // Biến lưu th�?i gian trước đó
float temperature;
float humidity;
uint16_t dust;
// uint16_t airperssure;
// uint16_t temperature;
// uint16_t humidity;
// uint16_t nodeid;
uint16_t longitude;
uint16_t latitude;
char nodeid[10] = "B1EFRT6"; // nodeid là chuỗi


/*Cảm biến ÁP suất BMP280*/
BMP280_HandleTypedef bmp280;
bmp280_params_t bmp280_params;

float airperssure;
float temperature_BMP280;
void I2C_ResetBus(I2C_HandleTypeDef *hi2c)
{
  if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BUSY))
  {
    HAL_I2C_DeInit(hi2c); // Hủy cấu hình hiện tại của I2C
    HAL_Delay(10);        // Chờ một thời gian để ổn định
    HAL_I2C_Init(hi2c);   // Khởi tạo lại I2C
  }
}
/*Cảm biến ÁP suất BMP280*/

/*RTC DS3231*/
#define DS3231_ADDRESS 0xD0

// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(int val)
{
  return (uint8_t)((val / 10 * 16) + (val % 10));
}
// Convert binary coded decimal to normal decimal numbers
int bcdToDec(uint8_t val)
{
  return (int)((val / 16 * 10) + (val % 16));
}

typedef struct
{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hour;
  uint8_t dayofweek;
  uint8_t dayofmonth;
  uint8_t month;
  uint8_t year;
} TIME_T;

TIME_T time;
// function to set time
void Set_Time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
  uint8_t set_time[7];
  set_time[0] = decToBcd(sec);
  set_time[1] = decToBcd(min);
  set_time[2] = decToBcd(hour);
  set_time[3] = decToBcd(dow);
  set_time[4] = decToBcd(dom);
  set_time[5] = decToBcd(month);
  set_time[6] = decToBcd(year);

  HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}
void Get_Time(void)
{
  uint8_t get_time[7];
  HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
  time.seconds = bcdToDec(get_time[0]);
  time.minutes = bcdToDec(get_time[1]);
  time.hour = bcdToDec(get_time[2]);
  time.dayofweek = bcdToDec(get_time[3]);
  time.dayofmonth = bcdToDec(get_time[4]);
  time.month = bcdToDec(get_time[5]);
  time.year = bcdToDec(get_time[6]);
}
float Get_Temp(void)
{
  uint8_t temp[2];

  HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x11, 1, temp, 2, 1000);
  return ((temp[0]) + (temp[1] >> 6) / 4.0);
}
void force_temp_conv(void)
{
  uint8_t status = 0;
  uint8_t control = 0;
  HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100); // read status register
  if (!(status & 0x04))
  {
    HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x0E, 1, &control, 1, 100); // read control register
    HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x0E, 1, (uint8_t *)(control | (0x20)), 1, 100);
  }
}
float TEMP;
char bufferDS3231[30];
/*RTC DS3231*/

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
  previousTick = HAL_GetTick(); // Ghi nhận th�?i gian bắt đầu kiểm tra

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
    HAL_Delay(1000); // �?ợi 1 giây trước khi thử lại
  }

  if (!APNisOK) // Nếu không thành công sau th�?i gian ch�?
  {
    HAL_UART_Transmit(&huart6, (uint8_t *)"APN configuration failed.\r\n", strlen("APN configuration failed.\r\n"), 1000);
  }
}

void httpPost(void)
{
  // Cấu hình HTTP
  SIMTransmit("AT+HTTPINIT\r\n"); // Khởi tạo HTTP
  HAL_Delay(2000);                // �?ợi một chút sau khi khởi tạo

  // Cấu hình URL (Server và Endpoint)
  sprintf(ATcommand, "AT+HTTPPARA=\"URL\",\"https://%s%s\"\r\n", server, resource);
  SIMTransmit(ATcommand); // Cấu hình URL của server
  HAL_Delay(2000);

  // Cấu hình loại nội dung (Content-Type)
  SIMTransmit("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r\n");
  HAL_Delay(2000);

  // Xác định kích thước dữ liệu POST
  sprintf(ATcommand, "AT+HTTPDATA=%d,10000\r\n", strlen(content)); // Th�?i gian ch�? 10000ms
  SIMTransmit(ATcommand);
  HAL_Delay(2000);

  // Gửi dữ liệu POST
  SIMTransmit(content); // Nội dung sẽ được gửi dưới dạng body của POST
  HAL_Delay(2000);

  // Gửi lệnh POST đến server
  SIMTransmit("AT+HTTPACTION=1\r\n"); // 1 là HTTP POST
  HAL_Delay(5000);                    // �?ợi phản hồi từ server

  // Kiểm tra mã trạng thái HTTP từ server (200 OK)
  if (strstr((char *)buffer, "+HTTPACTION: 1,200"))
  {
    HAL_UART_Transmit(&huart6, (uint8_t *)"POST success!!!!\r\n", strlen("POST success.\r\n"), 1000);
  }
  else
  {
    HAL_UART_Transmit(&huart6, (uint8_t *)"POST failed.\r\n", strlen("POST failed.\r\n"), 1000);
  }

  // �?óng HTTP
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  SHT31_Config(SHT31_ADDRESS_A, &hi2c1);
  // Kiểm tra và reset bus I2C nếu có lỗi
  I2C_ResetBus(&hi2c1);
  // Đảm bảo rằng I2C không còn bận trước khi tiếp tục
  while (hi2c1.State != HAL_I2C_STATE_READY)
  {
    // Có thể thêm một delay nhỏ nếu cần thiết
    HAL_Delay(1); // Thử lại sau 1ms
  }

  bmp280.i2c = &hi2c1;                // hi2c1 là biến I2C đã khởi tạo
  bmp280.addr = BMP280_I2C_ADDRESS_0; // hoặc BME280_I2C_ADDRESS_1

  // Khởi tạo các tham số mặc định cho BMP280
  bmp280_init_default_params(&bmp280_params);

  // Khởi tạo BMP280
  if (!bmp280_init(&bmp280, &bmp280_params))
  {
    // Xử lý lỗi khởi tạo
    while (1)
      ;
  }

  // Khởi tạo RTC
  rtc_init(&hi2c1);

  HAL_Delay(2000);
  checkAPN(); // Kiểm tra APN trước khi sử dụng HTTP POST


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // temperature = rand() % 41 + 20; // random giá trị từ 20 đến 60
    // humidity = rand() % 51 + 50;    // random giá trị từ 50 đến 100
    dust = 101;            // random giá trị từ 0 đến 100
    airperssure = 30; // random giá trị từ 30 đến 50
    longitude = 100;
    latitude = 200;
    // dust = rand() % 101;            // random giá trị từ 0 đến 100
    // airperssure = rand() % 21 + 30; // random giá trị từ 30 đến 50
    // longitude = rand() % 201+100;
    // latitude = rand() % 201+100;
    if (SHT31_GetData(SHT31_Periodic, SHT31_Medium, SHT31_NON_Stretch, SHT31_1) == SHT31_OK)
    {
      temperature = SHT31_GetTemperature();
      humidity = SHT31_GetHumidity();
    }
    HAL_Delay(2000);
    bmp280_read_float(&bmp280, &temperature_BMP280, &airperssure);
    sprintf(content, "temperature=%.2f&humidity=%.2f&dust=%d&airperssure=%.2f&nodeid=%s&longitude=%d&latitude=%d",
            temperature, humidity, dust, airperssure, nodeid, longitude, latitude);
    // sprintf(content, "temperature=%.2f&humidity=%.2f&dust=%d&airperssure=%d&nodeid=%s&longitude=%d&latitude=%d",
    //         temperature, humidity, dust, airperssure, nodeid, longitude, latitude);
    // sprintf(content, "temperature=%d&humidity=%d&dust=%d&airperssure=%d",temperature, humidity, dust, airperssure);
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
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
