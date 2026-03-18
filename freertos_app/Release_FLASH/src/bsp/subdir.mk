################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bsp/adc.c \
../src/bsp/can.c \
../src/bsp/led.c 

OBJS += \
./src/bsp/adc.o \
./src/bsp/can.o \
./src/bsp/led.o 

C_DEPS += \
./src/bsp/adc.d \
./src/bsp/can.d \
./src/bsp/led.d 


# Each subdirectory must supply rules for building sources it contributes
src/bsp/%.o: ../src/bsp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/bsp/adc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


