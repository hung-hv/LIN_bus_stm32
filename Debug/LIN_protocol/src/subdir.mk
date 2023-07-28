################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LIN_protocol/src/lin_protocol.c 

OBJS += \
./LIN_protocol/src/lin_protocol.o 

C_DEPS += \
./LIN_protocol/src/lin_protocol.d 


# Each subdirectory must supply rules for building sources it contributes
LIN_protocol/src/%.o LIN_protocol/src/%.su LIN_protocol/src/%.cyclo: ../LIN_protocol/src/%.c LIN_protocol/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LIN_protocol-2f-src

clean-LIN_protocol-2f-src:
	-$(RM) ./LIN_protocol/src/lin_protocol.cyclo ./LIN_protocol/src/lin_protocol.d ./LIN_protocol/src/lin_protocol.o ./LIN_protocol/src/lin_protocol.su

.PHONY: clean-LIN_protocol-2f-src

