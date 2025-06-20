################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/LIB/BH1750/BH1750.c 

OBJS += \
./Core/LIB/BH1750/BH1750.o 

C_DEPS += \
./Core/LIB/BH1750/BH1750.d 


# Each subdirectory must supply rules for building sources it contributes
Core/LIB/BH1750/%.o Core/LIB/BH1750/%.su Core/LIB/BH1750/%.cyclo: ../Core/LIB/BH1750/%.c Core/LIB/BH1750/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-LIB-2f-BH1750

clean-Core-2f-LIB-2f-BH1750:
	-$(RM) ./Core/LIB/BH1750/BH1750.cyclo ./Core/LIB/BH1750/BH1750.d ./Core/LIB/BH1750/BH1750.o ./Core/LIB/BH1750/BH1750.su

.PHONY: clean-Core-2f-LIB-2f-BH1750

