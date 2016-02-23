GCC_BIN = 
PROJECT = lono
OBJECTS = ./modules/Wireless.o ./modules/BatteryLevel.o ./modules/UI.o ./modules/PressureSensor.o ./modules/PCRxTask.o ./SDFileSystem/SDFileSystem.o ./SDFileSystem/FATFileSystem/FATDirHandle.o ./SDFileSystem/FATFileSystem/FATFileHandle.o ./SDFileSystem/FATFileSystem/FATFileSystem.o ./SDFileSystem/FATFileSystem/ChaN/ccsbcs.o ./SDFileSystem/FATFileSystem/ChaN/diskio.o ./SDFileSystem/FATFileSystem/ChaN/ff.o ./modules/SDCard.o  ./modules/util.o ./modules/PCSerial.o ./main.o ./modules/GSM.o ./modules/FourWireSerial.o ./rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/HAL_CM3.o ./rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/SVC_Table.o ./rtx/TARGET_CORTEX_M/HAL_CM.o ./rtx/TARGET_CORTEX_M/RTX_Conf_CM.o ./rtx/TARGET_CORTEX_M/rt_CMSIS.o ./rtx/TARGET_CORTEX_M/rt_Event.o ./rtx/TARGET_CORTEX_M/rt_List.o ./rtx/TARGET_CORTEX_M/rt_Mailbox.o ./rtx/TARGET_CORTEX_M/rt_MemBox.o ./rtx/TARGET_CORTEX_M/rt_Mutex.o ./rtx/TARGET_CORTEX_M/rt_Robin.o ./rtx/TARGET_CORTEX_M/rt_Semaphore.o ./rtx/TARGET_CORTEX_M/rt_System.o ./rtx/TARGET_CORTEX_M/rt_Task.o ./rtx/TARGET_CORTEX_M/rt_Time.o ./rtos/Mutex.o ./rtos/RtosTimer.o ./rtos/Semaphore.o ./rtos/Thread.o ./modules/XBEE.o
SYS_OBJECTS = ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o 
INCLUDE_PATHS = -I. -I./modules -I./rtos -I./rtx/TARGET_CORTEX_M -I./rtx/TARGET_CORTEX_M/TARGET_M3 -I./rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC -I./mbed -I./mbed/TARGET_LPC1768 -I./mbed/TARGET_LPC1768/TARGET_NXP -I./mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X -I./mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 -I./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM -I./SDFileSystem -I./SDFileSystem/FATFileSystem -I./SDFileSystem/FATFileSystem/ChaN
LIBRARY_PATHS = -L./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM
LIBRARIES = -lmbed
LINKER_SCRIPT = ./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld
ODEL = $(OBJECTS)

############################################################################### 
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-objdump
SIZE    = $(GCC_BIN)arm-none-eabi-size 


CPU = -mcpu=cortex-m3 -mthumb 
CC_FLAGS = $(CPU)  -std=c++11 -c -g -fno-common -fmessage-length=0 -Wextra -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP 
CC_SYMBOLS = -DDEBUG -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -DARM_MATH_CM3 -DTARGET_CORTEX_M -DTARGET_LPC176X -DTARGET_NXP -DTARGET_MBED_LPC1768 -DTARGET_LPC1768 -D__CORTEX_M3 -DTARGET_M3 -DMBED_BUILD_TIMESTAMP=1452169262.5 -D__MBED__=1 

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float -u _scanf_float -Wl,--wrap,main -Wl,-Map=$(PROJECT).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys


ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

.PHONY: all clean lst size

all: $(PROJECT).bin $(PROJECT).hex size


clean:
	rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst $(OBJECTS) $(DEPS)


.asm.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.s.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.S.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<

.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++11 -fno-rtti $(INCLUDE_PATHS) -o $@ $<



$(PROJECT).elf: $(OBJECTS) $(SYS_OBJECTS)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)


$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

lst: $(PROJECT).lst

size: $(PROJECT).elf
	$(SIZE) $(PROJECT).elf

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)

all: 	
	@echo Deleting files
	del $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst *.o *.d "modules\*.d" "modules\*.o" "SDFileSystem\*.d" "SDFileSystem\*.o" "SDFileSystem\FATFileSystem\*.d" "SDFileSystem\FATFileSystem\*.o" "SDFileSystem\FATFileSystem\ChaN\*.d" "SDFileSystem\FATFileSystem\ChaN\*.o"
	@echo Moving bin to bin directory and copying to board
	move "$(PROJECT).bin" "bin\$(PROJECT).bin"
	copy "bin\$(PROJECT).bin" "F:\"