# Building OBS Background Filter

This guide provides detailed instructions for building the OBS Background Filter plugin from source.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Dependency Installation](#dependency-installation)
3. [Building on Linux](#building-on-linux)
4. [Building on Windows](#building-on-windows)
5. [Building on macOS](#building-on-macos)
6. [Troubleshooting](#troubleshooting)

## Prerequisites

All platforms require:
- CMake 3.16 or higher
- Git
- C++17 compatible compiler
- OBS Studio development files
- OpenCV 4.x
- ONNX Runtime 1.12+ (optional but recommended)

## Dependency Installation

### Linux (Ubuntu/Debian)

```bash
# Essential build tools
sudo apt-get update
sudo apt-get install -y build-essential cmake git pkg-config

# OBS Studio development files
sudo apt-get install -y libobs-dev obs-studio

# OpenCV
sudo apt-get install -y libopencv-dev

# Qt (for OBS plugin interface)
sudo apt-get install -y qtbase5-dev qtbase5-private-dev

# Optional: CUDA for GPU acceleration
sudo apt-get install -y nvidia-cuda-toolkit
```

### Linux (Fedora/RHEL)

```bash
# Essential build tools
sudo dnf install -y gcc gcc-c++ cmake git pkgconfig

# OBS Studio development files
sudo dnf install -y obs-studio-devel

# OpenCV
sudo dnf install -y opencv-devel

# Qt
sudo dnf install -y qt5-qtbase-devel qt5-qtbase-private-devel
```

### Linux (Arch/CachyOS)

```bash
# All required packages in one command
sudo pacman -S base-devel cmake git pkgconf obs-studio opencv qt6-base openssl

# Package breakdown:
# - base-devel: C++ compiler and build tools
# - cmake: Build system  
# - git: Version control
# - pkgconf: Provides pkg-config for finding OBS libraries (REQUIRED!)
# - obs-studio: OBS itself + development headers
# - opencv: Computer vision library
# - qt6-base: Qt6 framework (OBS 28.0+ requires Qt6, NOT Qt5!)
# - openssl: Cryptographic library for SHA-256 verification
```

**⚠️ Important for Arch/CachyOS:**
- OBS Studio 28.0+ uses **Qt6**, not Qt5 (as of June 2023)
- `pkgconf` is **required** - provides `pkg-config` command used by CMake
- CachyOS repos include optimized builds

### Windows

1. **Visual Studio 2019 or 2022**
   - Download from: https://visualstudio.microsoft.com/
   - Install "Desktop development with C++" workload
   - Include CMake tools

2. **Git for Windows**
   - Download from: https://git-scm.com/download/win

3. **OBS Studio Source**
   ```powershell
   git clone --recursive https://github.com/obsproject/obs-studio.git
   cd obs-studio
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   cmake --build . --config Release
   ```

4. **OpenCV**
   - Download pre-built binaries: https://opencv.org/releases/
   - Extract to `C:\opencv`
   - Add to PATH: `C:\opencv\build\x64\vc15\bin`

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git opencv qt@6

# Install Xcode Command Line Tools
xcode-select --install

# OBS Studio
brew install --cask obs
# For development headers, build from source:
git clone --recursive https://github.com/obsproject/obs-studio.git
```

## ONNX Runtime Setup

ONNX Runtime is required for ML inference. Download from the official releases:

### Linux

```bash
# Download ONNX Runtime
wget https://github.com/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-linux-x64-1.15.1.tgz
tar -xzf onnxruntime-linux-x64-1.15.1.tgz
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-linux-x64-1.15.1
```

### Windows

```powershell
# Download from: https://github.com/microsoft/onnxruntime/releases
# Extract to desired location
# Set environment variable
$env:ONNXRUNTIME_ROOT_DIR = "C:\path\to\onnxruntime-win-x64-1.15.1"
```

### macOS

```bash
# Download ONNX Runtime
curl -L -O https://github.com/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-osx-x86_64-1.15.1.tgz
tar -xzf onnxruntime-osx-x86_64-1.15.1.tgz
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-osx-x86_64-1.15.1
```

## Building on Linux

### Quick Build

```bash
# Clone repository
git clone https://github.com/yourusername/OBS-background-filter.git
cd OBS-background-filter

# Set ONNX Runtime path (if downloaded)
export ONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime

# Run build script
chmod +x scripts/build.sh
./scripts/build.sh

# Install
cd build
sudo make install
```

### Manual Build

```bash
# Clone repository
git clone https://github.com/yourusername/OBS-background-filter.git
cd OBS-background-filter

# Create build directory
mkdir build && cd build

# Configure
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DOpenCV_DIR=/usr/lib/cmake/opencv4

# Build
make -j$(nproc)

# Install
sudo make install
```

### Custom Installation Path

For local installation without root:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime \
  -DCMAKE_INSTALL_PREFIX=$HOME/.config/obs-studio/plugins

make -j$(nproc)
make install
```

## Building on Windows

### Using Build Script

```powershell
# Clone repository
git clone https://github.com/yourusername/OBS-background-filter.git
cd OBS-background-filter

# Set environment variables
$env:ONNXRUNTIME_ROOT_DIR = "C:\path\to\onnxruntime"
$env:LibObs_DIR = "C:\path\to\obs-studio\build\libobs"
$env:OpenCV_DIR = "C:\opencv\build"

# Run build script (from Developer Command Prompt)
scripts\build.bat
```

### Manual Build

```powershell
# Clone repository
git clone https://github.com/yourusername/OBS-background-filter.git
cd OBS-background-filter

# Create build directory
mkdir build
cd build

# Configure (adjust paths as needed)
cmake .. `
  -G "Visual Studio 16 2019" `
  -A x64 `
  -DCMAKE_BUILD_TYPE=Release `
  -DONNXRUNTIME_ROOT_DIR="C:\path\to\onnxruntime" `
  -DLibObs_DIR="C:\path\to\obs-studio\build\libobs" `
  -DOpenCV_DIR="C:\opencv\build"

# Build
cmake --build . --config Release --parallel

# Install
cmake --install . --config Release
```

### Copy Runtime DLLs

After building, copy required DLLs to OBS plugin directory:

```powershell
# Copy to OBS plugins folder
copy "%ONNXRUNTIME_ROOT_DIR%\lib\onnxruntime.dll" "C:\Program Files\obs-studio\obs-plugins\64bit\"
copy "%OpenCV_DIR%\x64\vc15\bin\opencv_world4XX.dll" "C:\Program Files\obs-studio\obs-plugins\64bit\"
```

## Building on macOS

### Using Build Script

```bash
# Clone repository
git clone https://github.com/yourusername/OBS-background-filter.git
cd OBS-background-filter

# Set ONNX Runtime path
export ONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime

# Run build script
chmod +x scripts/build.sh
./scripts/build.sh

# Install
cd build
sudo make install
```

### Manual Build

```bash
# Clone repository
git clone https://github.com/yourusername/OBS-background-filter.git
cd OBS-background-filter

# Create build directory
mkdir build && cd build

# Configure
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime \
  -DCMAKE_INSTALL_PREFIX=/Applications/OBS.app/Contents \
  -DOpenCV_DIR=/usr/local/opt/opencv/lib/cmake/opencv4

# Build
make -j$(sysctl -n hw.ncpu)

# Install
sudo make install
```

## Building Without ONNX Runtime

If you want to build without ML inference (for testing structure only):

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Note**: The plugin will compile but ML inference will be disabled.

## Troubleshooting

### CMake Cannot Find OBS

**Error**: `Could not find libobs`

**Solution**:
```bash
# Linux: Install obs-studio development package
sudo apt-get install libobs-dev

# Or specify path manually
cmake .. -DLIBOBS_INCLUDE_DIR=/path/to/obs/libobs
```

### CMake Cannot Find OpenCV

**Error**: `Could not find OpenCV`

**Solution**:
```bash
# Specify OpenCV directory
cmake .. -DOpenCV_DIR=/usr/lib/cmake/opencv4

# Or install OpenCV
sudo apt-get install libopencv-dev  # Linux
brew install opencv                  # macOS
```

### ONNX Runtime Linking Errors

**Error**: `undefined reference to Ort::`

**Solution**:
- Ensure ONNXRUNTIME_ROOT_DIR points to correct directory
- Verify library files exist in `$ONNXRUNTIME_ROOT_DIR/lib`
- Check architecture matches (x64 vs x86)

### Plugin Not Appearing in OBS

**Checklist**:
1. Plugin installed in correct directory?
   - Linux: `/usr/lib/obs-plugins/`
   - Windows: `C:\Program Files\obs-studio\obs-plugins\64bit\`
   - macOS: `/Applications/OBS.app/Contents/PlugIns/`

2. Runtime dependencies available?
   - Check with `ldd` (Linux) or `otool -L` (macOS)
   - Copy missing DLLs on Windows

3. Check OBS logs:
   - Help → Log Files → View Current Log
   - Look for plugin loading errors

### Performance Issues

**Solutions**:
- Enable GPU acceleration (install CUDA/OpenCL)
- Use optimized ONNX Runtime build
- Reduce video resolution in OBS
- Use quantized model (INT8/FP16)

## Advanced Build Options

### Enable Debug Symbols

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Enable GPU Acceleration

```bash
# NVIDIA CUDA
cmake .. -DUSE_CUDA=ON -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda

# Intel OpenVINO
cmake .. -DUSE_OPENVINO=ON -DOpenVINO_DIR=/opt/intel/openvino
```

### Static Linking

```bash
cmake .. -DBUILD_STATIC=ON
```

## Verification

After building and installing:

1. Launch OBS Studio
2. Check Help → Log Files → View Current Log for plugin loading
3. Right-click video source → Filters → Add "AI Background Removal"
4. If filter appears, build was successful!

## Getting Help

If you encounter issues:
1. Check [GitHub Issues](https://github.com/yourusername/OBS-background-filter/issues)
2. Review OBS Studio build documentation
3. Verify all dependencies are correctly installed
4. Post detailed error messages and logs when asking for help

