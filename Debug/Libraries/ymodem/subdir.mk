################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/ymodem/ymodem.c 

OBJS += \
./Libraries/ymodem/ymodem.o 

C_DEPS += \
./Libraries/ymodem/ymodem.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/ymodem/%.o Libraries/ymodem/%.su Libraries/ymodem/%.cyclo: ../Libraries/ymodem/%.c Libraries/ymodem/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Peripheral/gpio -I"D:/BKHN/STM32/Bootloader/Peripheral/uart" -I"D:/BKHN/STM32/Bootloader/Libraries/ymodem" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries-2f-ymodem

clean-Libraries-2f-ymodem:
	-$(RM) ./Libraries/ymodem/ymodem.cyclo ./Libraries/ymodem/ymodem.d ./Libraries/ymodem/ymodem.o ./Libraries/ymodem/ymodem.su

.PHONY: clean-Libraries-2f-ymodem

