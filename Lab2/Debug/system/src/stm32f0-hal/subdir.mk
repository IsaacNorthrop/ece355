################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/stm32f0-hal/stm32f0xx_hal.c \
../system/src/stm32f0-hal/stm32f0xx_hal_cortex.c \
../system/src/stm32f0-hal/stm32f0xx_hal_flash.c \
../system/src/stm32f0-hal/stm32f0xx_hal_flash_ex.c \
../system/src/stm32f0-hal/stm32f0xx_hal_gpio.c \
../system/src/stm32f0-hal/stm32f0xx_hal_iwdg.c \
../system/src/stm32f0-hal/stm32f0xx_hal_pwr.c \
../system/src/stm32f0-hal/stm32f0xx_hal_rcc.c \
../system/src/stm32f0-hal/stm32f0xx_hal_tsc.c 

OBJS += \
./system/src/stm32f0-hal/stm32f0xx_hal.o \
./system/src/stm32f0-hal/stm32f0xx_hal_cortex.o \
./system/src/stm32f0-hal/stm32f0xx_hal_flash.o \
./system/src/stm32f0-hal/stm32f0xx_hal_flash_ex.o \
./system/src/stm32f0-hal/stm32f0xx_hal_gpio.o \
./system/src/stm32f0-hal/stm32f0xx_hal_iwdg.o \
./system/src/stm32f0-hal/stm32f0xx_hal_pwr.o \
./system/src/stm32f0-hal/stm32f0xx_hal_rcc.o \
./system/src/stm32f0-hal/stm32f0xx_hal_tsc.o 

C_DEPS += \
./system/src/stm32f0-hal/stm32f0xx_hal.d \
./system/src/stm32f0-hal/stm32f0xx_hal_cortex.d \
./system/src/stm32f0-hal/stm32f0xx_hal_flash.d \
./system/src/stm32f0-hal/stm32f0xx_hal_flash_ex.d \
./system/src/stm32f0-hal/stm32f0xx_hal_gpio.d \
./system/src/stm32f0-hal/stm32f0xx_hal_iwdg.d \
./system/src/stm32f0-hal/stm32f0xx_hal_pwr.d \
./system/src/stm32f0-hal/stm32f0xx_hal_rcc.d \
./system/src/stm32f0-hal/stm32f0xx_hal_tsc.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/stm32f0-hal/%.o: ../system/src/stm32f0-hal/%.c system/src/stm32f0-hal/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F051x8 -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f0-hal" -std=gnu11 -Wno-unused-parameter -Wno-conversion -Wno-sign-conversion -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


