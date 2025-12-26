#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Define paths
LOCAL_ROUTES_DIR="${SCRIPT_DIR}/../../../../data/routes"
HOST_ROUTES_DIR="/data/routes"

echo "========== Setting up Host Environment =========="

# 1. Setup /data/routes on the host machine
if [ ! -d "${HOST_ROUTES_DIR}" ]; then
    echo "Creating ${HOST_ROUTES_DIR}..."
    sudo mkdir -p "${HOST_ROUTES_DIR}"
    sudo chmod 777 "${HOST_ROUTES_DIR}"
fi

# 2. Copy route files
if [ -d "${LOCAL_ROUTES_DIR}" ]; then
    echo "Copying route files from ${LOCAL_ROUTES_DIR} to ${HOST_ROUTES_DIR}..."
    sudo cp "${LOCAL_ROUTES_DIR}"/*.csv "${HOST_ROUTES_DIR}/" 2>/dev/null || echo "No .csv files found."
else
    echo "⚠️  Local routes directory not found at: ${LOCAL_ROUTES_DIR}"
fi

echo "========== Building fogpass-core =========="

cd "${SCRIPT_DIR}/.."

if [ -d "build" ]; then
    sudo rm -rf build
fi

mkdir build
cd build

cmake ..
make -j$(nproc)