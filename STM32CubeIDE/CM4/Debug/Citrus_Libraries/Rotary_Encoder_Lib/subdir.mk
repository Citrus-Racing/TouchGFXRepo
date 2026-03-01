################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.c 

C_DEPS += \
./Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.d 

OBJS += \
./Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.o 


# Each subdirectory must supply rules for building sources it contributes
Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.o: C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.c Citrus_Libraries/Rotary_Encoder_Lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Flash_Storage_Lib" -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Structs" -I../../../CM4/Core/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Citrus_Libraries-2f-Rotary_Encoder_Lib

clean-Citrus_Libraries-2f-Rotary_Encoder_Lib:
	-$(RM) ./Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.cyclo ./Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.d ./Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.o ./Citrus_Libraries/Rotary_Encoder_Lib/CR_encoder.su

.PHONY: clean-Citrus_Libraries-2f-Rotary_Encoder_Lib

