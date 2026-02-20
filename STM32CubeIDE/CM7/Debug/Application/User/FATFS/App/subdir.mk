################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/CM7/FATFS/App/fatfs.c 

C_DEPS += \
./Application/User/FATFS/App/fatfs.d 

OBJS += \
./Application/User/FATFS/App/fatfs.o 


# Each subdirectory must supply rules for building sources it contributes
Application/User/FATFS/App/fatfs.o: C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/CM7/FATFS/App/fatfs.c Application/User/FATFS/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H757xx -c -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/Citrus_Libraries/CAN_Bus_Lib" -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/Citrus_Libraries/Flash_Storage_Lib" -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/Citrus_Libraries/Rotary_Encoder_Lib" -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/Citrus_Libraries/Shift_Light_Lib" -I"C:/Users/victo/OneDrive/Documents/Programming/TouchGFXTestRepo/Citrus_Libraries/Structs" -I../../../CM7/Core/Inc -I../../../CM7/FATFS/Target -I../../../CM7/FATFS/App -I../../../CM7/TouchGFX/App -I../../../CM7/TouchGFX/target/generated -I../../../CM7/TouchGFX/target -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Utilities/JPEG -I../../../Middlewares/Third_Party/FatFs/src -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -I../../CM7/../../CM7/Middlewares/ST/touchgfx/framework/include -I../../CM7/../../CM7/TouchGFX/generated/fonts/include -I../../CM7/../../CM7/TouchGFX/generated/gui_generated/include -I../../CM7/../../CM7/TouchGFX/generated/images/include -I../../CM7/../../CM7/TouchGFX/generated/texts/include -I../../CM7/../../CM7/TouchGFX/generated/videos/include -I../../CM7/../../CM7/TouchGFX/gui/include -I../../../CM7/USB_HOST/App -I../../../CM7/USB_HOST/Target -I../../../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-FATFS-2f-App

clean-Application-2f-User-2f-FATFS-2f-App:
	-$(RM) ./Application/User/FATFS/App/fatfs.cyclo ./Application/User/FATFS/App/fatfs.d ./Application/User/FATFS/App/fatfs.o ./Application/User/FATFS/App/fatfs.su

.PHONY: clean-Application-2f-User-2f-FATFS-2f-App

