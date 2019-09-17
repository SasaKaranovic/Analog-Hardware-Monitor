TARGET=SteamPunkDashboard
EXECUTABLE=SteamPunkDashboard.elf

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
SZ=arm-none-eabi-size

BIN=$(CP) -O ihex 

DEFS = -DUSE_HAL_DRIVER -DSTM32F103xB -DHSE_VALUE=8000000 -DDEBUG_BUILD=1
STARTUP = Device/startup_stm32f103xb.s

MCU = cortex-m3
MCFLAGS = -mcpu=$(MCU) -mthumb -mlittle-endian -mthumb-interwork

STM32_INCLUDES = -IApplication \
-IDevice \
-Iusb \
-ILib/lib_MCP4728 \
-IDevice/STM32F1xx_HAL_Driver/Inc \
-IDevice/STM32F1xx_HAL_Driver/Inc \
-IDevice/STM32_USB_Device_Library/Core/Inc \
-IDevice/STM32_USB_Device_Library/Class/CDC/Inc \
-IDevice/CMSIS/Device/ST/STM32F1xx/Include \
-IDevice/CMSIS/Include

# OPTIMIZE       = -O2 -Wall -Wextra -Wunused-parameter -Wno-sequence-point
OPTIMIZE       = -O2 -Wall -Wextra -Wunused-parameter -Wno-sequence-point -Wshadow -std=c99 -fdata-sections -ffunction-sections
# OPTIMIZE       = -std=gnu99 -fno-common -fmessage-length=0 -Wall -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -fdump-rtl-expand -Os -fdiagnostics-color -fstack-usage

CFLAGS	= $(MCFLAGS) $(OPTIMIZE) $(DEFS) -I. -I./ $(STM32_INCLUDES) -Wl,-T,Device/STM32F103C8Tx_FLASH.ld
AFLAGS	= $(MCFLAGS) 

SRC =  \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
Device/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
Device/STM32_USB_Device_Library/Core/Src/usbd_core.c \
Device/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
Device/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
Device/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
usb/usb_device.c \
usb/usbd_conf.c \
usb/usbd_desc.c \
usb/usbd_cdc_if.c \
Lib/lib_MCP4728/mcp4728.c \
Lib/lib_MCP4728/stm32f103_mcp4728_Driver.c \
Application/stm32f1xx_hal_msp.c \
Application/system_stm32f1xx.c \
Application/stm32f1xx_it.c \
Application/debug.c  \
Application/sys_init.c  \
Application/i2c.c  \
Application/uart.c  \
Application/uart_interface.c  \
Application/main.c 

OBJDIR = .
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) 
OBJ += Startup.o

all: $(TARGET).hex
	$(SZ) $(TARGET).elf

$(TARGET).hex: $(EXECUTABLE)
	$(CP) -O ihex $^ $@



$(EXECUTABLE): $(SRC) $(STARTUP)
	$(CC) $(CFLAGS) $^ -lm -lc -lnosys  -o $@

clean:
	del /s /q $(TARGET).elf
	del /s /q $(TARGET).hex


flash:
	ST-LINK_CLI.exe -c SWD UR -P "$(TARGET).hex" -V -HardRst

jlink:
	JLink -device STM32F103C8 -speed 4000 -if swd -autoconnect 1 -CommanderScript flash.jlink

analyze:
	nm --print-size --size-sort --reverse-sort --radix=d $(TARGET).elf > $(TARGET).size.txt