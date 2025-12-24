!/bin/bash
#
# This script deploys the FogpassUI application to the target hardware for testing.
#
# Usage:
#   ./deploy.sh [path/to/build_directory]
#
# If the build directory is not provided, it defaults to './build'.
#

set -e

BUILD_DIR=${1:-build}
EXECUTABLE_NAME="apptrial1"
SOURCE_FILE="${BUILD_DIR}/${EXECUTABLE_NAME}"

TARGET_USER="root"
TARGET_HOST="10.42.0.41"
TARGET_PATH="/home/root/project_test"

if [ ! -f "${SOURCE_FILE}" ]; then
    echo "Error: Executable '${SOURCE_FILE}' not found."
    echo "Please ensure the project is built and you are providing the correct build directory."
    exit 1
fi

echo "Deploying '${SOURCE_FILE}' to '${TARGET_USER}@${TARGET_HOST}:${TARGET_PATH}'..."
scp "${SOURCE_FILE}" "${TARGET_USER}@${TARGET_HOST}:${TARGET_PATH}/"
echo "Deployment successful."