#!/bin/bash
# Build script for OBS Background Filter Plugin (Linux/macOS)

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== OBS Background Filter Build Script ===${NC}\n"

# Configuration
BUILD_TYPE="${BUILD_TYPE:-Release}"
ONNXRUNTIME_DIR="${ONNXRUNTIME_ROOT_DIR:-}"
INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX:-/usr}"

# Detect OS for plugin paths
if [[ "$OSTYPE" == "darwin"* ]]; then
    USER_PLUGIN_DIR="$HOME/Library/Application Support/obs-studio/plugins"
    SYSTEM_PLUGIN_LIB="/usr/local/lib/obs-plugins"
    SYSTEM_PLUGIN_DATA="/usr/local/share/obs/obs-plugins"
else
    USER_PLUGIN_DIR="$HOME/.config/obs-studio/plugins"
    SYSTEM_PLUGIN_LIB="/usr/lib/obs-plugins"
    SYSTEM_PLUGIN_DATA="/usr/share/obs/obs-plugins"
fi

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo -e "${RED}Error: cmake is required but not installed.${NC}" >&2; exit 1; }
command -v g++ >/dev/null 2>&1 || command -v clang++ >/dev/null 2>&1 || { echo -e "${RED}Error: C++ compiler is required.${NC}" >&2; exit 1; }
command -v pkg-config >/dev/null 2>&1 || { echo -e "${YELLOW}Warning: pkg-config not found. May have issues finding OBS headers.${NC}"; }

# Check for OBS development headers
if ! pkg-config --exists libobs 2>/dev/null; then
    echo -e "${YELLOW}Warning: OBS development headers not found. Install libobs-dev or obs-studio-dev package.${NC}"
fi

echo -e "${YELLOW}Build Configuration:${NC}"
echo "  Build Type: $BUILD_TYPE"
echo "  Install Prefix: $INSTALL_PREFIX"
echo "  ONNX Runtime: ${ONNXRUNTIME_DIR:-Not specified}"
echo "  User Plugin Dir: $USER_PLUGIN_DIR"
echo "  System Plugin Dir: $SYSTEM_PLUGIN_LIB"
echo ""

# Check for ONNX Runtime
if [ -z "$ONNXRUNTIME_DIR" ]; then
    echo -e "${YELLOW}Warning: ONNXRUNTIME_ROOT_DIR not set. Plugin will compile without ML inference.${NC}"
    echo "Set ONNXRUNTIME_ROOT_DIR environment variable to enable ML features."
    echo "Example: export ONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime"
    echo ""
    read -p "Continue without ONNX Runtime? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Create build directory
echo -e "${GREEN}Creating build directory...${NC}"
mkdir -p build
cd build

# Configure with CMake
echo -e "${GREEN}Configuring with CMake...${NC}"
CMAKE_ARGS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
)

if [ -n "$ONNXRUNTIME_DIR" ]; then
    CMAKE_ARGS+=("-DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_DIR")
fi

cmake .. "${CMAKE_ARGS[@]}"

# Build
echo -e "${GREEN}Building...${NC}"
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -j$NPROC

# Verify build
PLUGIN_NAME="obs-background-filter"
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLUGIN_BINARY="$PLUGIN_NAME.so"
else
    PLUGIN_BINARY="$PLUGIN_NAME.so"
fi

if [ ! -f "$PLUGIN_BINARY" ]; then
    echo -e "${RED}Error: Plugin binary not found after build!${NC}"
    exit 1
fi

echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "Plugin binary: $(pwd)/$PLUGIN_BINARY\n"

# Installation instructions
echo -e "${YELLOW}Installation Options:${NC}"
echo ""
echo "1. System-wide installation (requires root):"
if [ "$INSTALL_PREFIX" = "/usr" ] || [ "$INSTALL_PREFIX" = "/usr/local" ]; then
    echo "   sudo make install"
else
    echo "   make install"
fi
echo ""
echo "2. User installation (no root required):"
echo "   mkdir -p \"$USER_PLUGIN_DIR/$PLUGIN_NAME/bin\""
echo "   mkdir -p \"$USER_PLUGIN_DIR/$PLUGIN_NAME/data\""
echo "   cp $PLUGIN_BINARY \"$USER_PLUGIN_DIR/$PLUGIN_NAME/bin/\""
echo "   cp -r ../data/* \"$USER_PLUGIN_DIR/$PLUGIN_NAME/data/\" 2>/dev/null || true"

echo -e "\n${YELLOW}Post-Installation Steps:${NC}"
echo "  1. Place your ONNX model at:"
echo "     $USER_PLUGIN_DIR/$PLUGIN_NAME/data/models/u2net.onnx"
echo "  2. If using system install:"
echo "     $SYSTEM_PLUGIN_DATA/$PLUGIN_NAME/models/u2net.onnx"
echo "  3. Restart OBS Studio"
echo "  4. Find the filter under 'Effect Filters' when right-clicking a source"

