################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/fonts/font5x7.c \
../Core/fonts/gfx_text.c \
../Core/fonts/st7735_gfx_adapter.c \
../Core/fonts/u8font_unscii16.c \
../Core/fonts/u8font_unscii8.c 

OBJS += \
./Core/fonts/font5x7.o \
./Core/fonts/gfx_text.o \
./Core/fonts/st7735_gfx_adapter.o \
./Core/fonts/u8font_unscii16.o \
./Core/fonts/u8font_unscii8.o 

C_DEPS += \
./Core/fonts/font5x7.d \
./Core/fonts/gfx_text.d \
./Core/fonts/st7735_gfx_adapter.d \
./Core/fonts/u8font_unscii16.d \
./Core/fonts/u8font_unscii8.d 


# Each subdirectory must supply rules for building sources it contributes
Core/fonts/%.o Core/fonts/%.su Core/fonts/%.cyclo: ../Core/fonts/%.c Core/fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I"/home/gerojohn/MicroSTM-OS/microstm-os/Core/Inc" -I"/home/gerojohn/MicroSTM-OS/microstm-os/Drivers/Custom/ST7735" -I"/home/gerojohn/MicroSTM-OS/microstm-os/Core/fonts" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-fonts

clean-Core-2f-fonts:
	-$(RM) ./Core/fonts/font5x7.cyclo ./Core/fonts/font5x7.d ./Core/fonts/font5x7.o ./Core/fonts/font5x7.su ./Core/fonts/gfx_text.cyclo ./Core/fonts/gfx_text.d ./Core/fonts/gfx_text.o ./Core/fonts/gfx_text.su ./Core/fonts/st7735_gfx_adapter.cyclo ./Core/fonts/st7735_gfx_adapter.d ./Core/fonts/st7735_gfx_adapter.o ./Core/fonts/st7735_gfx_adapter.su ./Core/fonts/u8font_unscii16.cyclo ./Core/fonts/u8font_unscii16.d ./Core/fonts/u8font_unscii16.o ./Core/fonts/u8font_unscii16.su ./Core/fonts/u8font_unscii8.cyclo ./Core/fonts/u8font_unscii8.d ./Core/fonts/u8font_unscii8.o ./Core/fonts/u8font_unscii8.su

.PHONY: clean-Core-2f-fonts

