################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32f4xx.c \
../system/src/cmsis/vectors_stm32f407xx.c 

OBJS += \
./system/src/cmsis/system_stm32f4xx.o \
./system/src/cmsis/vectors_stm32f407xx.o 

C_DEPS += \
./system/src/cmsis/system_stm32f4xx.d \
./system/src/cmsis/vectors_stm32f407xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/system_stm32f4xx.o: ../system/src/cmsis/system_stm32f4xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra -g3 -DDEBUG -DUSE_FULL_ASSERT -DOS_USE_SEMIHOSTING -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"C:\Users\assas\Desktop\FreeRTOS_Tasks_Communication\FreeRTOS_Tasks_Communication\FreeRTOS Source\include" -I"C:\Users\assas\Desktop\FreeRTOS_Tasks_Communication\FreeRTOS_Tasks_Communication\FreeRTOS Source\portable\GCC\ARM_CM4F" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -Wno-padded -MMD -MP -MF"$(@:%.o=%.d)" -MT"system/src/cmsis/system_stm32f4xx.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-move-loop-invariants -Wall -Wextra -g3 -DDEBUG -DUSE_FULL_ASSERT -DOS_USE_SEMIHOSTING -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"C:\Users\assas\Desktop\FreeRTOS_Tasks_Communication\FreeRTOS_Tasks_Communication\FreeRTOS Source\include" -I"C:\Users\assas\Desktop\FreeRTOS_Tasks_Communication\FreeRTOS_Tasks_Communication\FreeRTOS Source\portable\GCC\ARM_CM4F" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

