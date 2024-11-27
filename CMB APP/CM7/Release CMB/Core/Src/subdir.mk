################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/MQTTClient.c \
../Core/Src/MQTTTopic.c \
../Core/Src/PMB_comm.c \
../Core/Src/cJSON.c \
../Core/Src/cJSON_Utils.c \
../Core/Src/flash_if.c \
../Core/Src/freertos.c \
../Core/Src/main.c \
../Core/Src/sensirion_common.c \
../Core/Src/sensirion_i2c.c \
../Core/Src/serial_comm.c \
../Core/Src/sht4x.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_hal_timebase_tim.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/tws_driver.c \
../Core/Src/uart_callbacks.c \
../Core/Src/udpclient.c 

OBJS += \
./Core/Src/MQTTClient.o \
./Core/Src/MQTTTopic.o \
./Core/Src/PMB_comm.o \
./Core/Src/cJSON.o \
./Core/Src/cJSON_Utils.o \
./Core/Src/flash_if.o \
./Core/Src/freertos.o \
./Core/Src/main.o \
./Core/Src/sensirion_common.o \
./Core/Src/sensirion_i2c.o \
./Core/Src/serial_comm.o \
./Core/Src/sht4x.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_hal_timebase_tim.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/tws_driver.o \
./Core/Src/uart_callbacks.o \
./Core/Src/udpclient.o 

C_DEPS += \
./Core/Src/MQTTClient.d \
./Core/Src/MQTTTopic.d \
./Core/Src/PMB_comm.d \
./Core/Src/cJSON.d \
./Core/Src/cJSON_Utils.d \
./Core/Src/flash_if.d \
./Core/Src/freertos.d \
./Core/Src/main.d \
./Core/Src/sensirion_common.d \
./Core/Src/sensirion_i2c.d \
./Core/Src/serial_comm.d \
./Core/Src/sht4x.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_hal_timebase_tim.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/tws_driver.d \
./Core/Src/uart_callbacks.d \
./Core/Src/udpclient.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DCORE_CM7 -DCMB_HW -DUSER_VECT_TAB_ADDRESS -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I../LWIP/App -I../LWIP/Target -I../../Middlewares/Third_Party/LwIP/src/include -I../../Middlewares/Third_Party/LwIP/system -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Middlewares/Third_Party/LwIP/src/include/lwip -I../../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../Middlewares/Third_Party/LwIP/src/include/netif -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../../Middlewares/Third_Party/LwIP/system/arch -I../../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/MQTTClient.cyclo ./Core/Src/MQTTClient.d ./Core/Src/MQTTClient.o ./Core/Src/MQTTClient.su ./Core/Src/MQTTTopic.cyclo ./Core/Src/MQTTTopic.d ./Core/Src/MQTTTopic.o ./Core/Src/MQTTTopic.su ./Core/Src/PMB_comm.cyclo ./Core/Src/PMB_comm.d ./Core/Src/PMB_comm.o ./Core/Src/PMB_comm.su ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/cJSON_Utils.cyclo ./Core/Src/cJSON_Utils.d ./Core/Src/cJSON_Utils.o ./Core/Src/cJSON_Utils.su ./Core/Src/flash_if.cyclo ./Core/Src/flash_if.d ./Core/Src/flash_if.o ./Core/Src/flash_if.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/sensirion_common.cyclo ./Core/Src/sensirion_common.d ./Core/Src/sensirion_common.o ./Core/Src/sensirion_common.su ./Core/Src/sensirion_i2c.cyclo ./Core/Src/sensirion_i2c.d ./Core/Src/sensirion_i2c.o ./Core/Src/sensirion_i2c.su ./Core/Src/serial_comm.cyclo ./Core/Src/serial_comm.d ./Core/Src/serial_comm.o ./Core/Src/serial_comm.su ./Core/Src/sht4x.cyclo ./Core/Src/sht4x.d ./Core/Src/sht4x.o ./Core/Src/sht4x.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_hal_timebase_tim.cyclo ./Core/Src/stm32h7xx_hal_timebase_tim.d ./Core/Src/stm32h7xx_hal_timebase_tim.o ./Core/Src/stm32h7xx_hal_timebase_tim.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/tws_driver.cyclo ./Core/Src/tws_driver.d ./Core/Src/tws_driver.o ./Core/Src/tws_driver.su ./Core/Src/uart_callbacks.cyclo ./Core/Src/uart_callbacks.d ./Core/Src/uart_callbacks.o ./Core/Src/uart_callbacks.su ./Core/Src/udpclient.cyclo ./Core/Src/udpclient.d ./Core/Src/udpclient.o ./Core/Src/udpclient.su

.PHONY: clean-Core-2f-Src

