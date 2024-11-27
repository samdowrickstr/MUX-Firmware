################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.c \
../Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.c 

OBJS += \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.o \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.o 

C_DEPS += \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.d \
./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/MQTT/MQTTPacket/%.o Middlewares/Third_Party/MQTT/MQTTPacket/%.su Middlewares/Third_Party/MQTT/MQTTPacket/%.cyclo: ../Middlewares/Third_Party/MQTT/MQTTPacket/%.c Middlewares/Third_Party/MQTT/MQTTPacket/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../Core/Inc -I../LWIP/App -I../LWIP/Target -I../../Middlewares/Third_Party/LwIP/src/include -I../../Middlewares/Third_Party/LwIP/system -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Middlewares/Third_Party/LwIP/src/include/lwip -I../../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../Middlewares/Third_Party/LwIP/src/include/netif -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../../Middlewares/Third_Party/LwIP/system/arch -I../../Drivers/CMSIS/Include -I"C:/Users/Sam.Dowrick/Documents/Firmware/MUX APP/CM7/Middlewares/Third_Party/MQTT" -I"C:/Users/Sam.Dowrick/Documents/Firmware/MUX APP/CM7/Middlewares/Third_Party/MQTT/MQTTPacket" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-MQTT-2f-MQTTPacket

clean-Middlewares-2f-Third_Party-2f-MQTT-2f-MQTTPacket:
	-$(RM) ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectClient.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTConnectServer.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTDeserializePublish.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTFormat.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTPacket.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSerializePublish.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeClient.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTSubscribeServer.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeClient.su ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.cyclo ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.d ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.o ./Middlewares/Third_Party/MQTT/MQTTPacket/MQTTUnsubscribeServer.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-MQTT-2f-MQTTPacket

