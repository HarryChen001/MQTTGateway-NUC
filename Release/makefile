################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SOURCE/subdir.mk
-include LIB/common/sqlite3pp/subdir.mk
-include LIB/common/libserial/subdir.mk
-include LIB/common/cJSON/subdir.mk
-include LIB/common/base64/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: mqttgateway-v1.0

# Tool invocations
mqttgateway-v1.0: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-linux-g++ -L"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libaliyun/lib" -L"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libmodbus/lib" -L"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libsqlite/lib" -L"/home/ccl/eclipse-workspace/mqttgateway/LIB/arm-linux/libglog/lib" -o "mqttgateway-v1.0" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) mqttgateway-v1.0
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
