################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/LIB/TAU1201/TAU1201.c 

OBJS += \
./Core/LIB/TAU1201/TAU1201.o 

C_DEPS += \
./Core/LIB/TAU1201/TAU1201.d 


# Each subdirectory must supply rules for building sources it contributes
Core/LIB/TAU1201/%.o Core/LIB/TAU1201/%.su Core/LIB/TAU1201/%.cyclo: ../Core/LIB/TAU1201/%.c Core/LIB/TAU1201/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-LIB-2f-TAU1201

clean-Core-2f-LIB-2f-TAU1201:
	-$(RM) ./Core/LIB/TAU1201/TAU1201.cyclo ./Core/LIB/TAU1201/TAU1201.d ./Core/LIB/TAU1201/TAU1201.o ./Core/LIB/TAU1201/TAU1201.su

.PHONY: clean-Core-2f-LIB-2f-TAU1201

