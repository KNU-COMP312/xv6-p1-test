#!/bin/bash

# Check if PROJECT_HOME is set
if [ -z "${PROJECT_HOME}" ]; then
  echo "[ERROR] PROJECT_HOME is not set. Please export PROJECT_HOME before running this script."
  exit 1
fi

cd "${PROJECT_HOME}" || { echo "[ERROR] Failed to cd into PROJECT_HOME"; exit 1; }

# Argument check
TARGET="$1"
if [ -z "${TARGET}" ]; then
  echo "[ERROR] No target specified. Usage: $0 <target_name>"
  exit 1
fi

TEST_SCRIPT="${PROJECT_HOME}/test/test_${TARGET}.exp"
XV_HOME="${PROJECT_HOME}/xv6-riscv"

# Check if test script exists
if [ ! -f "${TEST_SCRIPT}" ]; then
  echo "[ERROR] Test script not found: ${TEST_SCRIPT}"
  exit 1
fi

# Check if xv6 directory exists
cd "${XV_HOME}" || { echo "[ERROR] Failed to cd into ${XV_HOME}"; exit 1; }

# Check if expect is installed
if ! command -v expect &> /dev/null; then
  echo "[INFO] 'expect' is not installed. Installing it now..."
  sudo apt update && sudo apt install -y expect
  if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to install expect. Please install it manually."
    exit 1
  fi
fi

# Build xv6
echo "[INFO] Cleaning xv6..."
cd ${XV_HOME}
make clean

# Run test
echo "[INFO] Running test script: ${TEST_SCRIPT}"
"${TEST_SCRIPT}"
