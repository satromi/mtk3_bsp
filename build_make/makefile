################################################################################
# micro T-Kernel 3.0 BSP  makefile
################################################################################

# Specify the name of the executable file to be built (Do not include the extension)
EXE_FILE := mtkernel_3

# Specify only one target name
#TARGET := _IOTE_M367_
#TARGET := _IOTE_RX231_
#TARGET := _IOTE_STM32L4_
#TARGET := _IOTE_RZA2M_
#TARGET := _NUCLEO_H723_
TARGET := _PICO_RP2040_

RM := rm -rf

# Workaround: GCC needs writable TEMP directory
export TEMP ?= /tmp
export TMP  ?= /tmp

INCPATH = -I"../include" -I"../config" -I"../kernel/knlinc" -I"../device/include"

-include sources.mk

include mtkernel_3/lib/libtm/subdir.mk
include mtkernel_3/lib/libtk/subdir.mk
include mtkernel_3/kernel/usermain/subdir.mk
include mtkernel_3/kernel/tstdlib/subdir.mk
include mtkernel_3/kernel/tkernel/subdir.mk
include mtkernel_3/kernel/sysinit/subdir.mk
include mtkernel_3/kernel/inittask/subdir.mk

include mtkernel_3/device/subdir.mk

ifeq ($(TARGET), _IOTE_M367_)
include iote_m367.mk
endif
ifeq ($(TARGET), _IOTE_RX231_)
include iote_rx231.mk
endif
ifeq ($(TARGET), _IOTE_STM32L4_)
include iote_stm32l4.mk
endif
ifeq ($(TARGET), _IOTE_RZA2M_)
include iote_rza2m.mk
endif
ifeq ($(TARGET), _NUCLEO_H723_)
include nucleo_h723.mk
endif
ifeq ($(TARGET), _PICO_RP2040_)
include pico_rp2040.mk
endif
ifeq ($(TARGET), _PICO_W_)
include pico_w.mk
endif

-include mtkernel_3/app_program/subdir.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Build target: RP2040/Pico W generates .elf + .uf2, others generate .elf only
ifneq (,$(filter $(TARGET), _PICO_RP2040_ _PICO_W_))
all: $(EXE_FILE).elf $(EXE_FILE).uf2
else
all: $(EXE_FILE).elf
	@echo ' '
endif

$(EXE_FILE).elf: $(OBJS)
	@echo 'Linker: $@'
	$(LINK)  $(LFLAGS) -T $(LNKFILE) -Wl,-Map,"$(EXE_FILE).map" -o "$(EXE_FILE).elf" $(OBJS) -lm
	@echo 'Finished building target: $@'
	@echo ' '

# Generate .uf2 for RP2040 BOOTSEL flashing (RP2040 only)
ELF2UF2 ?= C:/home/elf2uf2/elf2uf2.exe

$(EXE_FILE).uf2: $(EXE_FILE).elf
	$(ELF2UF2) $(EXE_FILE).elf $(EXE_FILE).uf2
	@echo 'Generated: $(EXE_FILE).uf2 (copy to RP2040 in BOOTSEL mode)'
	@echo ' '

# Other EXE_FILEs
clean:
	-$(RM) $(OBJS) $(SECONDARY_SIZE) $(ASM_DEPS) $(S_UPPER_DEPS) $(C_DEPS) $(EXE_FILE).elf $(EXE_FILE).bin $(EXE_FILE).uf2 $(EXE_FILE).map
	-@echo ' '

# Keymap name for output filenames (default: "default")
KEYMAP ?= default

# Build both master and slave firmware
split: split_master split_slave
	@echo ' '
	@echo '=== Both firmwares built (keymap=$(KEYMAP)) ==='
	@echo '  Master: $(EXE_FILE)_master_$(KEYMAP).uf2'
	@echo '  Slave:  $(EXE_FILE)_slave_$(KEYMAP).uf2'
	@echo ' '

split_master:
	@echo '=== Building MASTER firmware (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_RP2040_ clean
	$(MAKE) -j1 TARGET=_PICO_RP2040_ KEYMAP=$(KEYMAP) all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_master_$(KEYMAP).uf2
	@echo 'Master: $(EXE_FILE)_master_$(KEYMAP).uf2'
	@echo ' '

split_slave:
	@echo '=== Building SLAVE firmware (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_RP2040_ clean
	$(MAKE) -j1 TARGET=_PICO_RP2040_ KEYMAP=$(KEYMAP) EXTRA_DEFS="-DKB_IS_MASTER=0" all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_slave_$(KEYMAP).uf2
	@echo 'Slave: $(EXE_FILE)_slave_$(KEYMAP).uf2'
	@echo ' '

# ======================================================================
# BLE split: 左右両方 Pico W (BLE ワイヤレス接続)
# ======================================================================
split_ble: split_ble_master split_ble_slave
	@echo ' '
	@echo '=== Both BLE split firmwares built (keymap=$(KEYMAP)) ==='
	@echo '  Master (Pico W, BLE): $(EXE_FILE)_master_ble_$(KEYMAP).uf2'
	@echo '  Slave  (Pico W, BLE): $(EXE_FILE)_slave_ble_$(KEYMAP).uf2'

split_ble_master:
	@echo '=== Building BLE MASTER (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_W_ clean
	$(MAKE) -j1 TARGET=_PICO_W_ KEYMAP=$(KEYMAP) EXTRA_DEFS="-DSPLIT_TRANSPORT_BLE" all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_master_ble_$(KEYMAP).uf2

split_ble_slave:
	@echo '=== Building BLE SLAVE (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_W_ clean
	$(MAKE) -j1 TARGET=_PICO_W_ KEYMAP=$(KEYMAP) EXTRA_DEFS="-DKB_IS_MASTER=0 -DSPLIT_TRANSPORT_BLE" all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_slave_ble_$(KEYMAP).uf2

# ======================================================================
# Hybrid split: Master=Pico W (BLE+USB auto), Slave=任意 (I2C or BLE)
#   Master は起動時に I2C slave を probe し、なければ BLE scan
#   Slave は Pico W なら BLE、RP2040 なら I2C
# ======================================================================
split_auto: split_auto_master split_auto_slave_i2c
	@echo ' '
	@echo '=== Hybrid split firmwares built (keymap=$(KEYMAP)) ==='
	@echo '  Master (Pico W, auto): $(EXE_FILE)_master_auto_$(KEYMAP).uf2'
	@echo '  Slave  (RP2040, I2C):  $(EXE_FILE)_slave_$(KEYMAP).uf2'
	@echo '  (Slave に BLE 版を使う場合: make split_auto_slave_ble)'

split_auto_master:
	@echo '=== Building AUTO MASTER (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_W_ clean
	$(MAKE) -j1 TARGET=_PICO_W_ KEYMAP=$(KEYMAP) EXTRA_DEFS="-DSPLIT_TRANSPORT_AUTO" all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_master_auto_$(KEYMAP).uf2

split_auto_slave_i2c:
	@echo '=== Building I2C SLAVE (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_RP2040_ clean
	$(MAKE) -j1 TARGET=_PICO_RP2040_ KEYMAP=$(KEYMAP) EXTRA_DEFS="-DKB_IS_MASTER=0" all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_slave_$(KEYMAP).uf2

split_auto_slave_ble:
	@echo '=== Building BLE SLAVE (keymap=$(KEYMAP)) ==='
	$(MAKE) TARGET=_PICO_W_ clean
	$(MAKE) -j1 TARGET=_PICO_W_ KEYMAP=$(KEYMAP) EXTRA_DEFS="-DKB_IS_MASTER=0 -DSPLIT_TRANSPORT_BLE" all
	cp $(EXE_FILE).uf2 $(EXE_FILE)_slave_ble_$(KEYMAP).uf2

.PHONY: all clean dependents split split_master split_slave
.PHONY: split_ble split_ble_master split_ble_slave
.PHONY: split_auto split_auto_master split_auto_slave_i2c split_auto_slave_ble
