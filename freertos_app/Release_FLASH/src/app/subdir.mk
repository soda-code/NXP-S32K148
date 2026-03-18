################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app/adc_app.c \
../src/app/freertos_app.c \
../src/app/key_app.c \
../src/app/led_app.c 

OBJS += \
./src/app/adc_app.o \
./src/app/freertos_app.o \
./src/app/key_app.o \
./src/app/led_app.o 

C_DEPS += \
./src/app/adc_app.d \
./src/app/freertos_app.d \
./src/app/key_app.d \
./src/app/led_app.d 


# Each subdirectory must supply rules for building sources it contributes
src/app/%.o: ../src/app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/app/adc_app.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


