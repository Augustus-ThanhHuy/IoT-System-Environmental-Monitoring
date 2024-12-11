#include "stm32f4xx_hal.h"
#include "DS3231_test.h"

#define DS3231_ADDR (0x68 << 1)

I2C_HandleTypeDef *i2c;

void DS3231_Init(I2C_HandleTypeDef *handle){
    i2c = handle;
}

static uint8_t B2D(uint8_t bcd)
{
    return (bcd>>4)*10+(bcd&0x0f);
}

static uint8_t D2B(uint8_t dec)
{
    return (((dec/10)<<4) | (dec%10));
}

bool DS3231_set_time(TIME *rtc){
    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[8] = {startAddr, D2B(rtc->Sec), D2B(rtc->Min), D2B(rtc->Hour), D2B(rtc->DaysOfWeek), D2B(rtc->Date), D2B(rtc->Month), D2B(rtc->year)};
    if (HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;
    return true; 
}

bool DS3231_get_time(TIME *rtc){
    uint8_t startAddr = DS3231_REG_TIME;
    uint8_t buffer[7] = {
        0,
    };
    if (HAL_I2C_Master_Transmit(i2c, DS3231_ADDR, &startAddr, 1, HAL_MAX_DELAY) != HAL_OK)
        return false;
    if (HAL_I2C_Master_Receive(i2c, DS3231_ADDR, buffer, sizeof(buffer), HAL_MAX_DELAY) != HAL_OK)
        return false;

    rtc->Sec = B2D(buffer[0] & 0x7F);
    rtc->Min = B2D(buffer[1] & 0x7F);
    rtc->Hour = B2D(buffer[2] & 0x3F);
    rtc->DaysOfWeek = buffer[3] & 0x07;
    rtc->Date = B2D(buffer[4] & 0x3F);
    rtc->Month = B2D(buffer[5] & 0x1F);
    rtc->year = B2D(buffer[6]);
    return true;

}