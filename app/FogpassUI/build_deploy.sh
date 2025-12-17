#!/bin/bash
#
# FogpassUI build + deploy script
#
# This script:
#   1. Creates a build directory (if not exists)
#   2. Sources STM32 SDK environment
#   3. Configures the project using CMake
#   4. Builds the executable
#   5. Deploys it to the target hardware
#

set -e   # Exit immediately if any command fails

# ============================================================
# 🔧 CONFIGURABLE PARAMETERS (EDIT HERE ONLY)
# ============================================================

# Build
BUILD_DIR="build_stm"
EXECUTABLE_NAME="apptrial1"

# Qt / SDK

SDK_ENV_SCRIPT="/opt/stm/environment-setup-cortexa35-ostl-linux"
QT_HOST_PATH="/home/sachin/Qt/6.8.3/gcc_64"

# Target device
TARGET_USER="root"
TARGET_HOST="10.42.0.41"
TARGET_PATH="/home/root/project_test"

# ============================================================
# 🚀 SCRIPT START
# ============================================================

echo "========== FogpassUI Build & Deploy =========="

# ------------------------------------------------------------
# 1️⃣ Create build directory
# ------------------------------------------------------------
if [ ! -d "${BUILD_DIR}" ]; then
    echo "Creating build directory: ${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
fi

cd "${BUILD_DIR}"

# ------------------------------------------------------------
# 2️⃣ Source STM SDK environment
# ------------------------------------------------------------
if [ ! -f "${SDK_ENV_SCRIPT}" ]; then
    echo "Error: SDK environment script not found:"
    echo "  ${SDK_ENV_SCRIPT}"
    exit 1
fi

echo "Sourcing SDK environment..."
source "${SDK_ENV_SCRIPT}"

# ------------------------------------------------------------
# 3️⃣ Configure project with CMake
# ------------------------------------------------------------
echo "Running CMake configuration..."

cmake .. \
    -DQT_HOST_PATH="${QT_HOST_PATH}"

# ------------------------------------------------------------
# 4️⃣ Build the project
# ------------------------------------------------------------
echo "Building project..."
make -j$(nproc)

# ------------------------------------------------------------
# 5️⃣ Deploy to target hardware
# ------------------------------------------------------------
SOURCE_FILE="./${EXECUTABLE_NAME}"

if [ ! -f "${SOURCE_FILE}" ]; then
    echo "Error: Executable '${SOURCE_FILE}' not found."
    exit 1
fi

echo "Deploying '${EXECUTABLE_NAME}' to ${TARGET_USER}@${TARGET_HOST}:${TARGET_PATH}"
scp "${SOURCE_FILE}" "${TARGET_USER}@${TARGET_HOST}:${TARGET_PATH}/"

echo "✅ Deployment successful."
echo "============================================="
