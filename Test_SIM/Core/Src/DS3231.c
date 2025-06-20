#include "DS3231.h"

static I2C_HandleTypeDef *ds_i2c;

// Initialize the RTC with I2C
void rtc_init(I2C_HandleTypeDef *i2c)
{
    ds_i2c = i2c;
}

// Convert Decimal to BCD (Binary-Coded Decimal)
uint8_t Decimal2BCD(uint8_t num)
{
    return (num / 10) << 4 | (num % 10);
}

// Convert BCD to Decimal
uint8_t BCD2Decimal(uint8_t num)
{
    return (num >> 4) * 10 + (num & 0x0F);
}

// Set the current time for the RTC (Customizable)
void set_currently_timer_ds3231(DS3231_data_t *st)
{
    // Set the date and time (adjust as necessary)
    st->Second = 58;
    st->Minute = 1;
    st->Hour = 10;
    st->Day = 2;
    st->Date = 2;
    st->Month = 12;
    st->Year = 24; // Year 2024
}

// Write time to DS3231 RTC
void DS3231_write_time(DS3231_data_t *dt)
{
    uint8_t data_tran[8];
    data_tran[0] = 0x00; // Address to start at (seconds register)
    data_tran[1] = Decimal2BCD(dt->Second);
    data_tran[2] = Decimal2BCD(dt->Minute);
    data_tran[3] = Decimal2BCD(dt->Hour);
    data_tran[4] = Decimal2BCD(dt->Day);
    data_tran[5] = Decimal2BCD(dt->Date);
    data_tran[6] = Decimal2BCD(dt->Month);
    data_tran[7] = Decimal2BCD(dt->Year);

    // Transmit the data to the RTC
    HAL_I2C_Master_Transmit(ds_i2c, RTC_ADDR, data_tran, 8, 100);
}

// Read time from DS3231 RTC
void DS3231_read_time(DS3231_data_t *dt)
{
    uint8_t data_receive[7];
    uint8_t add_reg = 0x00; // Start reading from the seconds register

    // Write the register address to start reading from
    HAL_I2C_Master_Transmit(ds_i2c, RTC_ADDR, &add_reg, 1, 100);

    // Receive the time data (7 bytes)
    HAL_I2C_Master_Receive(ds_i2c, RTC_ADDR, data_receive, 7, 100);

    // Convert BCD to Decimal and store it in the struct
    dt->Second = BCD2Decimal(data_receive[0]);
    dt->Minute = BCD2Decimal(data_receive[1]);
    dt->Hour = BCD2Decimal(data_receive[2]);
    dt->Day = BCD2Decimal(data_receive[3]);
    dt->Date = BCD2Decimal(data_receive[4]);
    dt->Month = BCD2Decimal(data_receive[5]);
    dt->Year = BCD2Decimal(data_receive[6]);
}