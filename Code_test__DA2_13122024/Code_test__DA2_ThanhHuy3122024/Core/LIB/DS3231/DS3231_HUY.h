/*
 * DS3231_HUY.h
 *
 *  Created on: Dec 3, 2024
 *      Author: user
 */

#ifndef LIB_DS3231_DS3231_HUY_H_
#define LIB_DS3231_DS3231_HUY_H_


#include "stm32f4xx_hal.h"  // Adjust to your MCU's HAL header

// DS3231 I2C Address (Shifted by 1 bit for the read/write operation)
//#define RTC_ADDR (0x68 << 1)
#define RTC_ADDR 0xD0
// Structure to hold RTC date and time
typedef struct {
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Day;
    uint8_t Date;
    uint8_t Month;
    uint8_t Year;
} DS3231_data_t;

// Function prototypes
void rtc_init(I2C_HandleTypeDef *i2c);
void DS3231_write_time(DS3231_data_t *dt);
void DS3231_read_time(DS3231_data_t *dt);
void set_currently_timer_ds3231(DS3231_data_t *st);
uint8_t Decimal2BCD(uint8_t num);
uint8_t BCD2Decimal(uint8_t num);


#endif /* LIB_DS3231_DS3231_HUY_H_ */
