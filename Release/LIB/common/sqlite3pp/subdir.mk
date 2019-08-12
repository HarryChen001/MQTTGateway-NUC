################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LIB/common/sqlite3pp/sqlite3pp.cpp \
../LIB/common/sqlite3pp/sqlite3ppext.cpp 

OBJS += \
./LIB/common/sqlite3pp/sqlite3pp.o \
./LIB/common/sqlite3pp/sqlite3ppext.o 

CPP_DEPS += \
./LIB/common/sqlite3pp/sqlite3pp.d \
./LIB/common/sqlite3pp/sqlite3ppext.d 


# Each subdirectory must supply rules for building sources it contributes
LIB/common/sqlite3pp/%.o: ../LIB/common/sqlite3pp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-g++ --std=gnu++11 -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libaliyun/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/sqlite3pp" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libsqlite/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/common/libserial" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libmodbus/include" -I"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libglog/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


