################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../SOURCE/main.cpp \
../SOURCE/soft_aliyunmqtt.cpp \
../SOURCE/soft_myfunction.cpp \
../SOURCE/soft_mymodbus.cpp \
../SOURCE/soft_sqlite3pp.cpp 

OBJS += \
./SOURCE/main.o \
./SOURCE/soft_aliyunmqtt.o \
./SOURCE/soft_myfunction.o \
./SOURCE/soft_mymodbus.o \
./SOURCE/soft_sqlite3pp.o 

CPP_DEPS += \
./SOURCE/main.d \
./SOURCE/soft_aliyunmqtt.d \
./SOURCE/soft_myfunction.d \
./SOURCE/soft_mymodbus.d \
./SOURCE/soft_sqlite3pp.d 


# Each subdirectory must supply rules for building sources it contributes
SOURCE/%.o: ../SOURCE/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-g++ --std=gnu++11 -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libaliyun/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/sqlite3pp" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libsqlite/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/libserial" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libmodbus/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libglog/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


