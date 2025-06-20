################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/LIB/DS3231/DS3231_HUY.c 

OBJS += \
./Core/LIB/DS3231/DS3231_HUY.o 

C_DEPS += \
./Core/LIB/DS3231/DS3231_HUY.d 


# Each subdirectory must supply rules for building sources it contributes
Core/LIB/DS3231/%.o Core/LIB/DS3231/%.su Core/LIB/DS3231/%.cyclo: ../Core/LIB/DS3231/%.c Core/LIB/DS3231/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-LIB-2f-DS3231

clean-Core-2f-LIB-2f-DS3231:
	-$(RM) ./Core/LIB/DS3231/DS3231_HUY.cyclo ./Core/LIB/DS3231/DS3231_HUY.d ./Core/LIB/DS3231/DS3231_HUY.o ./Core/LIB/DS3231/DS3231_HUY.su

.PHONY: clean-Core-2f-LIB-2f-DS3231

