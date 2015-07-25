################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Buzzer.c \
../src/HD44780_F3.c \
../src/KeyPad.c \
../src/Sens.c \
../src/Timer.c \
../src/_write.c \
../src/main.c 

OBJS += \
./src/Buzzer.o \
./src/HD44780_F3.o \
./src/KeyPad.o \
./src/Sens.o \
./src/Timer.o \
./src/_write.o \
./src/main.o 

C_DEPS += \
./src/Buzzer.d \
./src/HD44780_F3.d \
./src/KeyPad.d \
./src/Sens.d \
./src/Timer.d \
./src/_write.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F30X -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/CMSIS" -I"../system/include/stm32f3-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


