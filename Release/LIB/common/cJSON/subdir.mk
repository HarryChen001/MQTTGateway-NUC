################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LIB/common/cJSON/cJSON.c 

OBJS += \
./LIB/common/cJSON/cJSON.o 

C_DEPS += \
./LIB/common/cJSON/cJSON.d 


# Each subdirectory must supply rules for building sources it contributes
LIB/common/cJSON/%.o: ../LIB/common/cJSON/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gcc --std=gnu11 -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libaliyun/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/sqlite3pp" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/libserial" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libmodbus/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libsqlite/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libglog/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


