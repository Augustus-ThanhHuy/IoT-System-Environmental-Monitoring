/* 
 * Author: Sano
 * Created on: 2024-10-19
 */
#ifndef DS3231_TEST_H
#define DS3231_TEST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DS3231_REG_TIME 0x00
#define DS3231_REG_ALARM1 0x07
#define DS3231_REG_ALARM2 0x0B
#define DS3231_REG_CONTROL 0x0E
#define DS3231_REG_STATUS 0x0F

typedef enum{
    SUNDAY = 1,
    MONDAY,
    TUESDAY,
    WENNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
}DaysOfWeek;

typedef struct
{
    uint8_t year;
    uint8_t Month;
    uint8_t Date;
    uint8_t DaysOfWeek;
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;
}TIME;

void DS3231_Init(I2C_HandleTypeDef *handle); 
bool DS3231_set_time(TIME *rtc);
bool DS3231_get_time(TIME *rtc);

#endif /* DS3231_TEST_H */