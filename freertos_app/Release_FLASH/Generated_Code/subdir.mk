################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/AMMCLib1.c \
../Generated_Code/Cpu.c \
../Generated_Code/adConv1.c \
../Generated_Code/clockMan1.c \
../Generated_Code/dmaController1.c \
../Generated_Code/pin_mux.c \
../Generated_Code/watchdog1.c 

OBJS += \
./Generated_Code/AMMCLib1.o \
./Generated_Code/Cpu.o \
./Generated_Code/adConv1.o \
./Generated_Code/clockMan1.o \
./Generated_Code/dmaController1.o \
./Generated_Code/pin_mux.o \
./Generated_Code/watchdog1.o 

C_DEPS += \
./Generated_Code/AMMCLib1.d \
./Generated_Code/Cpu.d \
./Generated_Code/adConv1.d \
./Generated_Code/clockMan1.d \
./Generated_Code/dmaController1.d \
./Generated_Code/pin_mux.d \
./Generated_Code/watchdog1.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/AMMCLib1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


