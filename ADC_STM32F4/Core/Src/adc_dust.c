#include "adc_dust.h"
#include "stdint.h"
#include "stm32f407xx.h"
// Khai báo các biến toàn cục đã được định nghĩa trong main.c
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
#define LED_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_3

// Định nghĩa thời gian trễ
#define DELAY_TIME 280 // microseconds
#define DELAY_TIME2 40 // microseconds
#define OFF_TIME 9680  // microseconds

uint32_t dustVal = 0;
float voltage = 0;
float dustDensity = 0;
/**
 * @brief Tạo độ trễ theo microsecond sử dụng Timer
 * @param delay Thời gian trễ tính bằng microseconds
 */
static void delay_us(uint16_t delay)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0); // Đặt lại bộ đếm về 0
    while (__HAL_TIM_GET_COUNTER(&htim2) > delay); // Chờ đến khi bộ đếm đạt giá trị delay
}

/**
 * @brief Khởi tạo module ADC (bao gồm ADC và Timer)
 */
void ADC_Module_Init(void)
{
    // Khởi tạo ADC và Timer đã được gọi trong main.c
    // Nếu cần thêm cấu hình, thực hiện tại đây
    // Ví dụ: bắt đầu Timer
    HAL_TIM_Base_Start(&htim2);
}

/**
 * @brief Đọc giá trị ADC từ cảm biến bụi
 * @return Giá trị ADC (0 - 4095 cho ADC 12-bit)
 */
uint32_t ADC_Module_ReadValue(void)
{
    uint32_t dustVal = 0;

    // Bật LED (Power on)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);      // HIHG: LED bật
    delay_us(DELAY_TIME);                                    // Delay 0.28ms

    // Đọc giá trị ADC
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
        dustVal = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);

    // Tắt LED
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); // LOW: LED tắt
    delay_us(DELAY_TIME2);
    delay_us(OFF_TIME);

    return dustVal;
}

/**
 * @brief Chuyển đổi giá trị ADC thành điện áp
 * @param adcValue Giá trị ADC đọc được
 * @return Điện áp tương ứng (V)
 */
float ADC_Module_GetVoltage(uint32_t adcValue)
{
    return ((float)adcValue) * (3.3f / 4095.0f); // STM32 ADC 12-bit: 0-4095 tương ứng 0-3.3V
}

/**
 * @brief Tính toán mật độ bụi từ điện áp
 * @param voltage Điện áp đọc được từ ADC
 * @return Mật độ bụi (mg/m³)
 */
float ADC_Module_GetDustDensity(float voltage)
{
    float dustDensity = 0.172f * voltage - 0.1f;

    if (dustDensity < 0.0f)
        dustDensity = 0.0f;
    if (dustDensity > 0.5f)
        dustDensity = 0.5f;

    return dustDensity;
}

/**
 * @brief Hàm tổng hợp để đọc và tính toán mật độ bụi
 * @return Mật độ bụi (mg/m³)
 */
float ADC_Module_ReadDustDensity(void)
{
    uint32_t adcValue = ADC_Module_ReadValue();
    voltage = ADC_Module_GetVoltage(adcValue);
    dustDensity = ADC_Module_GetDustDensity(voltage);
    return dustDensity;
}

