################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LIB/common/libserial/Serial.cpp \
../LIB/common/libserial/Stream.cpp \
../LIB/common/libserial/Timestamp.cpp 

OBJS += \
./LIB/common/libserial/Serial.o \
./LIB/common/libserial/Stream.o \
./LIB/common/libserial/Timestamp.o 

CPP_DEPS += \
./LIB/common/libserial/Serial.d \
./LIB/common/libserial/Stream.d \
./LIB/common/libserial/Timestamp.d 


# Each subdirectory must supply rules for building sources it contributes
LIB/common/libserial/%.o: ../LIB/common/libserial/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-g++ -std=c++0x -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libaliyun/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/rapidjson" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/sqlite3pp" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libsqlite/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/libserial" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libmodbus/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


