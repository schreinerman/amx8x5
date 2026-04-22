# AMX8X5 Arduino Test Suite
#
# Runs two kinds of checks:
#   1. Compilation tests  – compile every example sketch for multiple boards.
#      This verifies that the library at least compiles without errors.
#   2. Unit tests         – compile (and optionally upload/run) the AUnit test
#      sketch that exercises driver logic via mock I2C callbacks.
#
# Requirements:
#   arduino-cli  https://arduino.github.io/arduino-cli/latest/installation/
#
# Quick start:
#   make install-deps          # install cores + AUnit (once)
#   make                       # compile all examples + unit tests
#   make compile-tests-uno     # compile examples for Arduino Uno only
#   make unit-tests-rp2040     # compile unit tests for RP2040 only
#   make upload PROFILE=rp2040 PORT=/dev/ttyACM0  # upload & run on hardware
#
# Tip: set VERBOSE=1 to see full compiler output.
# ---------------------------------------------------------------------------

LIBRARY_PATH := $(abspath ..)
BUILD_DIR    := build
UNIT_TEST_DIR := unit-tests

# ---- Board FQBNs ----------------------------------------------------------
FQBN_UNO    := arduino:avr:uno
FQBN_RP2040 := rp2040:rp2040:rpipico
FQBN_ESP32  := esp32:esp32:esp32

# ---- Board manager URLs ---------------------------------------------------
URL_RP2040 := https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
URL_ESP32  := https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# ---- Compiler output: suppress unless VERBOSE=1 --------------------------
ifeq ($(VERBOSE),1)
  QUIET :=
else
  QUIET := > /dev/null 2>&1
endif

# ---- All example sketches (paths relative to repo root) ------------------
# Note: Arduino AVR (Uno, Mega …) is intentionally excluded from the
# compilation tests.  The example sketches use Serial.printf() which is
# available on ESP32, RP2040 and most modern platforms but not on AVR's
# HardwareSerial implementation.  I2C/SPI hardware is present on AVR, so
# hand-written AVR sketches using Serial.print() would compile fine.
EXAMPLES := \
    examples/basic/basic.ino \
    examples/timer-wakeup/timer-wakeup.ino \
    examples/exti-wakeup/exti-wakeup.ino \
    examples/alarm-wakeup/alarm-wakeup.ino \
    examples/watchdog/watchdog.ino \
    examples/autocalibration/autocalibration.ino \
    examples/digital-calibration/digital-calibration.ino \
    examples/ram-storage/ram-storage.ino \
    examples/square-wave-output/square-wave-output.ino \
    examples/trickle-charger/trickle-charger.ino \
    examples/rc-oscillator/rc-oscillator.ino \
    examples/i2c-basic/i2c-basic.ino \
    examples/spi-basic/spi-basic.ino \
    examples/output-pin-config/output-pin-config.ino \
    examples/oscillator-fail-reset/oscillator-fail-reset.ino \
    examples/iot-power-management/iot-power-management.ino

# ---------------------------------------------------------------------------
.PHONY: all install-deps \
        compile-tests compile-tests-uno compile-tests-rp2040 compile-tests-esp32 \
        unit-tests unit-tests-uno unit-tests-rp2040 unit-tests-esp32 \
        upload clean

all: compile-tests unit-tests

# ---- Install Arduino cores and the AUnit library -------------------------
install-deps:
	@echo "Installing Arduino cores..."
	arduino-cli core install arduino:avr
	arduino-cli core install rp2040:rp2040 --additional-urls "$(URL_RP2040)"
	arduino-cli core install esp32:esp32   --additional-urls "$(URL_ESP32)"
	@echo "Installing AUnit library..."
	arduino-cli lib install AUnit
	@echo "Done."

# ---- Helper: compile a single sketch for a given fqbn --------------------
# Called directly in shell for-loops below.
# Usage: compile_one <fqbn> <tag> <rel-sketch-path>
COMPILE = arduino-cli compile \
	--fqbn "$(1)" \
	--library "$(LIBRARY_PATH)" \
	--build-path "$(BUILD_DIR)/$(2)/$(notdir $(patsubst %/,%,$(dir $(3))))" \
	"../$(3)"

# ---- Compilation tests for all examples ----------------------------------
compile-tests: compile-tests-rp2040 compile-tests-esp32

compile-tests-uno:
	@echo "=== Compilation tests: Arduino Uno (AVR) ==="
	@mkdir -p $(addprefix $(BUILD_DIR)/uno/,$(notdir $(patsubst %/,%,$(dir $(EXAMPLES)))))
	@ok=0; fail=0; \
	for ex in $(EXAMPLES); do \
	  name=$$(basename $$(dirname $$ex)); \
	  printf "  [%-6s] %s ... " "uno" "$$ex"; \
	  if arduino-cli compile --fqbn "$(FQBN_UNO)" --library "$(LIBRARY_PATH)" \
	       --build-path "$(BUILD_DIR)/uno/$$name" "../$$ex" > /dev/null 2>&1; then \
	    echo "OK"; ok=$$((ok+1)); \
	  else \
	    echo "FAIL"; fail=$$((fail+1)); \
	  fi; \
	done; \
	echo ""; echo "  Results: $$ok passed, $$fail failed."; [ $$fail -eq 0 ]

compile-tests-rp2040:
	@echo "=== Compilation tests: RP2040 (Raspberry Pi Pico) ==="
	@mkdir -p $(addprefix $(BUILD_DIR)/rp2040/,$(notdir $(patsubst %/,%,$(dir $(EXAMPLES)))))
	@ok=0; fail=0; \
	for ex in $(EXAMPLES); do \
	  name=$$(basename $$(dirname $$ex)); \
	  printf "  [%-6s] %s ... " "rp2040" "$$ex"; \
	  if arduino-cli compile --fqbn "$(FQBN_RP2040)" --library "$(LIBRARY_PATH)" \
	       --build-path "$(BUILD_DIR)/rp2040/$$name" "../$$ex" > /dev/null 2>&1; then \
	    echo "OK"; ok=$$((ok+1)); \
	  else \
	    echo "FAIL"; fail=$$((fail+1)); \
	  fi; \
	done; \
	echo ""; echo "  Results: $$ok passed, $$fail failed."; [ $$fail -eq 0 ]

compile-tests-esp32:
	@echo "=== Compilation tests: ESP32 ==="
	@mkdir -p $(addprefix $(BUILD_DIR)/esp32/,$(notdir $(patsubst %/,%,$(dir $(EXAMPLES)))))
	@ok=0; fail=0; \
	for ex in $(EXAMPLES); do \
	  name=$$(basename $$(dirname $$ex)); \
	  printf "  [%-6s] %s ... " "esp32" "$$ex"; \
	  if arduino-cli compile --fqbn "$(FQBN_ESP32)" --library "$(LIBRARY_PATH)" \
	       --build-path "$(BUILD_DIR)/esp32/$$name" "../$$ex" > /dev/null 2>&1; then \
	    echo "OK"; ok=$$((ok+1)); \
	  else \
	    echo "FAIL"; fail=$$((fail+1)); \
	  fi; \
	done; \
	echo ""; echo "  Results: $$ok passed, $$fail failed."; [ $$fail -eq 0 ]

# ---- Unit tests (compile; use 'make upload' to run on hardware) -----------
unit-tests: unit-tests-rp2040 unit-tests-esp32

unit-tests-uno:
	@echo "=== Unit tests compile: Arduino Uno (AVR) ==="
	arduino-cli compile \
		--profile uno \
		--library "$(LIBRARY_PATH)" \
		--build-path "$(BUILD_DIR)/uno/$(UNIT_TEST_DIR)" \
		"$(UNIT_TEST_DIR)"

unit-tests-rp2040:
	@echo "=== Unit tests compile: RP2040 ==="
	arduino-cli compile \
		--profile rp2040 \
		--library "$(LIBRARY_PATH)" \
		--build-path "$(BUILD_DIR)/rp2040/$(UNIT_TEST_DIR)" \
		"$(UNIT_TEST_DIR)"

unit-tests-esp32:
	@echo "=== Unit tests compile: ESP32 ==="
	arduino-cli compile \
		--profile esp32 \
		--library "$(LIBRARY_PATH)" \
		--build-path "$(BUILD_DIR)/esp32/$(UNIT_TEST_DIR)" \
		"$(UNIT_TEST_DIR)"

# ---- Upload and run unit tests on a connected board ----------------------
# Usage: make upload PROFILE=rp2040 PORT=/dev/ttyACM0
upload:
ifndef PROFILE
	$(error PROFILE is not set. Usage: make upload PROFILE=rp2040 PORT=/dev/ttyACM0)
endif
ifndef PORT
	$(error PORT is not set. Usage: make upload PROFILE=rp2040 PORT=/dev/ttyACM0)
endif
	arduino-cli compile --upload \
		--profile "$(PROFILE)" \
		--library "$(LIBRARY_PATH)" \
		--port "$(PORT)" \
		"$(UNIT_TEST_DIR)"
	@echo ""
	@echo "Open serial monitor to see AUnit test results:"
	@echo "  arduino-cli monitor --port $(PORT) --config baudrate=115200"

# ---- Clean ---------------------------------------------------------------
clean:
	rm -rf "$(BUILD_DIR)"
	mkdir -p "$(BUILD_DIR)"
	@echo "Build directory cleaned."
