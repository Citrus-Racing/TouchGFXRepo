################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.c 

C_DEPS += \
./Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.d 

OBJS += \
./Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.o 


# Each subdirectory must supply rules for building sources it contributes
Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.o: C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.c Citrus_Libraries/Flash_Storage_Lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Flash_Storage_Lib" -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXRepo/Citrus_Libraries/Structs" -I../../../CM4/Core/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Citrus_Libraries-2f-Flash_Storage_Lib

clean-Citrus_Libraries-2f-Flash_Storage_Lib:
	-$(RM) ./Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.cyclo ./Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.d ./Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.o ./Citrus_Libraries/Flash_Storage_Lib/CR_flash_storage.su

.PHONY: clean-Citrus_Libraries-2f-Flash_Storage_Lib

