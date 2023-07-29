################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TestPart4.cpp \
../src/TreeNode.cpp \
../src/tokenClasses.cpp \
../src/tokenClassifier.cpp 

CPP_DEPS += \
./src/TestPart4.d \
./src/TreeNode.d \
./src/tokenClasses.d \
./src/tokenClassifier.d 

OBJS += \
./src/TestPart4.o \
./src/TreeNode.o \
./src/tokenClasses.o \
./src/tokenClassifier.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++2a -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/TestPart4.d ./src/TestPart4.o ./src/TreeNode.d ./src/TreeNode.o ./src/tokenClasses.d ./src/tokenClasses.o ./src/tokenClassifier.d ./src/tokenClassifier.o

.PHONY: clean-src

