#!/usr/bin/env bash
# run_tests.sh — AMX8X5 Arduino Test Runner
#
# Runs two kinds of checks without uploading to hardware:
#   1. Compilation tests  — compile every example sketch for each enabled board
#   2. Unit tests         — compile the AUnit test sketch for each enabled board
#
# Requirements:
#   arduino-cli  https://arduino.github.io/arduino-cli/latest/installation/
#   AUnit        arduino-cli lib install AUnit
#
# Usage:
#   ./run_tests.sh               # all boards
#   ./run_tests.sh rp2040        # only RP2040
#   ./run_tests.sh rp2040 esp32  # only RP2040 + ESP32
#
# Exit code: 0 if all tests pass, 1 otherwise.

set -uo pipefail

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="/tmp/amx8x5/build"
UNIT_TEST_DIR="${SCRIPT_DIR}/unit-tests"
LIB_PATH="${REPO_ROOT}"

# Board configurations: name -> FQBN
declare -A FQBN=(
    [rp2040]="rp2040:rp2040:rpipico"
    [esp32]="esp32:esp32:esp32"
)

# Example sketches (relative to repo root)
EXAMPLES=(
    examples/basic/basic.ino
    examples/timer-wakeup/timer-wakeup.ino
    examples/exti-wakeup/exti-wakeup.ino
    examples/alarm-wakeup/alarm-wakeup.ino
    examples/watchdog/watchdog.ino
    examples/autocalibration/autocalibration.ino
    examples/digital-calibration/digital-calibration.ino
    examples/ram-storage/ram-storage.ino
    examples/square-wave-output/square-wave-output.ino
    examples/trickle-charger/trickle-charger.ino
    examples/rc-oscillator/rc-oscillator.ino
    examples/i2c-basic/i2c-basic.ino
    examples/spi-basic/spi-basic.ino
    examples/output-pin-config/output-pin-config.ino
    examples/oscillator-fail-reset/oscillator-fail-reset.ino
    examples/iot-power-management/iot-power-management.ino
)

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m'

PASS=0
FAIL=0
FAIL_LIST=()

ok()   { printf "  ${GREEN}OK${NC}   %s\n" "$1"; ((PASS++)) || true; }
fail() { printf "  ${RED}FAIL${NC} %s\n" "$1"; ((FAIL++)) || true; FAIL_LIST+=("$1"); }

section() { printf "\n${BOLD}=== %s ===${NC}\n" "$*"; }

# ---------------------------------------------------------------------------
# Determine which boards to test
# ---------------------------------------------------------------------------

if [[ $# -gt 0 ]]; then
    BOARDS=("$@")
else
    BOARDS=("${!FQBN[@]}")
fi

# Validate requested boards
for b in "${BOARDS[@]}"; do
    if [[ -z "${FQBN[$b]+x}" ]]; then
        echo "Unknown board '${b}'. Available: ${!FQBN[*]}"
        exit 1
    fi
done

printf "${BOLD}AMX8X5 Arduino Test Runner${NC}\n"
printf "Repo   : %s\n" "${REPO_ROOT}"
printf "Boards : %s\n" "${BOARDS[*]}"

# ---------------------------------------------------------------------------
# Check prerequisites
# ---------------------------------------------------------------------------

section "Prerequisites"

if ! command -v arduino-cli &>/dev/null; then
    echo "arduino-cli not found. Install from https://arduino.github.io/arduino-cli/"
    exit 1
fi
echo "  arduino-cli $(arduino-cli version 2>/dev/null | awk '{print $3}')"

for b in "${BOARDS[@]}"; do
    fqbn="${FQBN[$b]}"
    core="${fqbn%%:*}:$(echo "$fqbn" | cut -d: -f2)"
    if ! arduino-cli core list 2>/dev/null | grep -q "^${core}"; then
        printf "  ${YELLOW}WARN${NC} Core '%s' not installed. Run: arduino-cli core install %s\n" "$core" "$core"
    else
        echo "  Core ${core} installed."
    fi
done

if ! arduino-cli lib list 2>/dev/null | grep -qi "^AUnit"; then
    printf "  ${YELLOW}WARN${NC} AUnit not installed. Run: arduino-cli lib install AUnit\n"
else
    echo "  AUnit installed."
fi

# ---------------------------------------------------------------------------
# Run compilation tests
# ---------------------------------------------------------------------------

compile_sketch() {
    local board="$1" sketch_rel="$2"
    local sketch_abs="${REPO_ROOT}/${sketch_rel}"
    local sketch_name
    sketch_name="$(basename "$(dirname "${sketch_abs}")")"
    local build_out="${BUILD_DIR}/${board}/${sketch_name}"

    mkdir -p "${build_out}"
    local log="${build_out}/build.log"

    if arduino-cli compile \
        --fqbn "${FQBN[$board]}" \
        --library "${LIB_PATH}" \
        --build-path "${build_out}" \
        "${sketch_abs}" >"${log}" 2>&1; then
        ok "[${board}] ${sketch_rel}"
    else
        fail "[${board}] ${sketch_rel}"
        # Show last 10 lines of error on failure
        sed -n 'p' "${log}" | tail -10 | sed 's/^/      /'
    fi
}

for board in "${BOARDS[@]}"; do
    section "Compilation tests: ${board} (${FQBN[$board]})"
    for sketch in "${EXAMPLES[@]}"; do
        compile_sketch "${board}" "${sketch}"
    done
done

# ---------------------------------------------------------------------------
# Run unit-test sketch compilation
# AUnit does not support ArduinoCore-API platforms (e.g. rp2040).
# Unit tests are compiled only for boards where AUnit works (esp32).
# ---------------------------------------------------------------------------
AUNIT_BOARDS=("esp32")

for board in "${AUNIT_BOARDS[@]}"; do
    # Skip if this board was not requested
    skip=1
    for b in "${BOARDS[@]}"; do [[ "$b" == "$board" ]] && skip=0; done
    [[ $skip -eq 1 ]] && continue

    section "Unit tests compile: ${board} (${FQBN[$board]})"
    build_out="${BUILD_DIR}/${board}/unit-tests"
    mkdir -p "${build_out}"
    log="${build_out}/build.log"

    if arduino-cli compile \
        --fqbn "${FQBN[$board]}" \
        --library "${LIB_PATH}" \
        --build-path "${build_out}" \
        "${UNIT_TEST_DIR}" >"${log}" 2>&1; then
        ok "[${board}] unit-tests"
    else
        fail "[${board}] unit-tests"
        sed -n 'p' "${log}" | tail -10 | sed 's/^/      /'
    fi
done

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------

section "Summary"
printf "  ${GREEN}Passed${NC}: %d\n" "${PASS}"
printf "  ${RED}Failed${NC}: %d\n" "${FAIL}"

if [[ ${FAIL} -gt 0 ]]; then
    printf "\nFailed tests:\n"
    for t in "${FAIL_LIST[@]}"; do
        printf "  - %s\n" "$t"
    done
    exit 1
fi

printf "\n${GREEN}All tests passed.${NC}\n"
exit 0
