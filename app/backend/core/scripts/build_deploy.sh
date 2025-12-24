#!/bin/bash
#
# fogpass-core build + deploy script
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
BUILD_DIR="../../build_core"
EXECUTABLE_NAME="fogpass-core"

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

echo "========== fogpass-core Build & Deploy =========="

# Get the directory where the script is located
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# ------------------------------------------------------------
# 1️⃣ Create build directory
# ------------------------------------------------------------
if [ ! -d "${SCRIPT_DIR}/${BUILD_DIR}" ]; then
    echo "Creating build directory: ${SCRIPT_DIR}/${BUILD_DIR}"
    mkdir -p "${SCRIPT_DIR}/${BUILD_DIR}"
fi

cd "${SCRIPT_DIR}/${BUILD_DIR}"

# ------------------------------------------------------------
# 2️⃣ Source STM SDK environment
# ------------------------------------------------------------
echo "Sourcing SDK environment..."
source "${SDK_ENV_SCRIPT}"

# ------------------------------------------------------------
# 3️⃣ Configure project with CMake
# ------------------------------------------------------------
echo "Running CMake configuration..."
cmake "${SCRIPT_DIR}/../" \
    -DQT_HOST_PATH="${QT_HOST_PATH}"

# ------------------------------------------------------------
# 4️⃣ Build and Deploy
# ------------------------------------------------------------
echo "Building and deploying project..."
make -j$(nproc) && \
    (
        echo "Stopping existing service on target..." && \
        ssh "${TARGET_USER}@${TARGET_HOST}" "killall ${EXECUTABLE_NAME} || true" && \
        echo "Deploying new executable..." && \
        scp "./${EXECUTABLE_NAME}" "${TARGET_USER}@${TARGET_HOST}:${TARGET_PATH}/"
    )

echo "✅ Deployment successful."
echo "============================================="