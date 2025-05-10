################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Peripheral/uart/usart.c 

OBJS += \
./Peripheral/uart/usart.o 

C_DEPS += \
./Peripheral/uart/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Peripheral/uart/%.o Peripheral/uart/%.su Peripheral/uart/%.cyclo: ../Peripheral/uart/%.c Peripheral/uart/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Peripheral/gpio -I"D:/BKHN/STM32/Bootloader/Peripheral/uart" -I"D:/BKHN/STM32/Bootloader/Libraries/ymodem" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Peripheral-2f-uart

clean-Peripheral-2f-uart:
	-$(RM) ./Peripheral/uart/usart.cyclo ./Peripheral/uart/usart.d ./Peripheral/uart/usart.o ./Peripheral/uart/usart.su

.PHONY: clean-Peripheral-2f-uart

