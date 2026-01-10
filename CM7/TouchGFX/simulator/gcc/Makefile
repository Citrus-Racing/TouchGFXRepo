# Helper macros to convert spaces into question marks and back again
e := 
sp := $(e) $(e)
qs = $(subst ?,$(sp),$1)
sq = $(subst $(sp),?,$1)

# Get name of this Makefile (avoid getting word 0 and a starting space)
makefile_name := $(wordlist 1,1000,$(MAKEFILE_LIST))

# Get path of this Makefile
makefile_path := $(call qs,$(dir $(call sq,$(abspath $(call sq,$(makefile_name))))))

# Get path where the Application is
application_path := $(call qs,$(abspath $(call sq,$(makefile_path)../..)))

.PHONY: clean assets all

ifneq ($(words $(makefile_path))$(words $(MAKEFILE_LIST)),11)
all clean assets:
$(error Spaces not allowed in path)
else
$(info makefile_path = '$(application_path)')
ADDITIONAL_SOURCES := 
ADDITIONAL_INCLUDE_PATHS := ../Core/Inc \
../FATFS/Target \
../FATFS/App \
../TouchGFX/App \
../TouchGFX/target/generated \
../TouchGFX/target \
../Middlewares/ST/touchgfx/framework/include \
../TouchGFX/generated/fonts/include \
../TouchGFX/generated/gui_generated/include \
../TouchGFX/generated/images/include \
../TouchGFX/generated/texts/include \
../TouchGFX/generated/videos/include \
../TouchGFX/gui/include \
../USB_HOST/App \
../USB_HOST/Target \
../../Drivers/STM32H7xx_HAL_Driver/Inc \
../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy \
../../Drivers/CMSIS/Device/ST/STM32H7xx/Include \
../../Drivers/CMSIS/Include \
../../Utilities/JPEG \
../../Middlewares/Third_Party/FatFs/src \
../../Middlewares/Third_Party/FreeRTOS/Source/include \
../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
../../Middlewares/ST/STM32_USB_Host_Library/Core/Inc \
../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc
ADDITIONAL_LIBRARY_PATHS := 
ADDITIONAL_LIBRARIES :=
export ADDITIONAL_SOURCES ADDITIONAL_INCLUDE_PATHS ADDITIONAL_LIBRARY_PATHS ADDITIONAL_LIBRARIES

all: $(filter assets,$(MAKECMDGOALS))
all assets: $(filter clean,$(MAKECMDGOALS))
all clean assets:
	@$(MAKE) -r -f generated/simulator/gcc/Makefile -s $(MFLAGS) $@ -C "$(application_path)"
endif

