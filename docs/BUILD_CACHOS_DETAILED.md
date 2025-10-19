# Building on CachyOS/Arch Linux

This guide is specifically for building the OBS Background Filter plugin on CachyOS and other Arch-based distributions.

## Why This Guide Exists

Arch-based distributions like CachyOS handle OBS differently than Debian-based systems:
- OBS doesn't provide CMake package files (`find_package` won't work)
- We use `pkg-config` instead to locate OBS headers and libraries
- Plugin installation paths are different

## Quick Build (5 Minutes)

### 1. Install Dependencies

```bash
# Install all required packages
sudo pacman -S base-devel cmake git pkgconf obs-studio opencv qt6-base openssl
```

**What each package does:**
- `base-devel`: C++ compiler and build tools
- `cmake`: Build system
- `git`: Version control
- `obs-studio`: OBS itself + development headers
- `opencv`: Computer vision library
- `qt6-base`: Qt framework (used by OBS)
- `openssl`: Cryptographic library (for SHA-256 verification)

### 2. Download ONNX Runtime

```bash
cd /home/crocboot/Projects/OBS-background-filter

# Download ONNX Runtime 1.19.2 (latest stable)
wget https://github.com/microsoft/onnxruntime/releases/download/v1.19.2/onnxruntime-linux-x64-1.19.2.tgz

# Extract
tar -xzf onnxruntime-linux-x64-1.19.2.tgz

# Set environment variable
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-linux-x64-1.19.2
```

### 3. Build the Plugin

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR

# Build (use all CPU cores)
make -j$(nproc)
```

**Expected output:**
```
-- Build Configuration Summary ===
Build Type: Release
Install Prefix: /home/crocboot/.config/obs-studio/plugins
OBS Libraries: obs
OpenCV Version: 4.x.x
ONNX Runtime: TRUE
ONNX Runtime Library: /path/to/libonnxruntime.so
===================================
```

### 4. Install

```bash
# Install to user directory (no sudo needed!)
make install
```

This installs to: `~/.config/obs-studio/plugins/obs-background-filter/`

### 5. Download Model

```bash
# Create models directory
mkdir -p ~/.config/obs-studio/plugins/obs-background-filter/data/models

# Download U2-Net model
wget https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2net.onnx \
  -O ~/.config/obs-studio/plugins/obs-background-filter/data/models/u2net.onnx
```

### 6. Launch OBS

```bash
obs
```

Then:
1. Right-click a video source → **Filters**
2. Add **"AI Background Removal"** under Effect Filters
3. Configure settings!

## Detailed Explanation

### How CMake Finds OBS on CachyOS

Our `CMakeLists.txt` uses `pkg-config`:

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBOBS REQUIRED libobs)
```

This works because the `obs-studio` package installs a `.pc` file:
```bash
$ pkg-config --cflags libobs
-I/usr/include/obs
```

### Installation Paths

By default, the plugin installs to your **user directory**:
```
~/.config/obs-studio/plugins/obs-background-filter/
├── bin/
│   └── obs-background-filter.so
└── data/
    ├── locale/
    │   └── en-US.ini
    └── models/
        └── u2net.onnx (you download this)
```

OBS automatically loads plugins from this location - no root access needed!

### Verifying Installation

```bash
# Check plugin binary exists
ls ~/.config/obs-studio/plugins/obs-background-filter/bin/

# Should show: obs-background-filter.so

# Check it's a proper shared library
file ~/.config/obs-studio/plugins/obs-background-filter/bin/obs-background-filter.so

# Should show: ELF 64-bit LSB shared object
```

### Checking OBS Logs

If the plugin doesn't appear:

```bash
# Launch OBS from terminal to see output
obs

# Or check logs
cat ~/.config/obs-studio/logs/$(ls -t ~/.config/obs-studio/logs/ | head -1) | grep -i background
```

Look for:
```
[obs-background-filter] Model loaded successfully
```

## Troubleshooting

### "pkg-config not found"

```bash
sudo pacman -S pkgconf
```

### "Could not find libobs"

```bash
# Verify OBS is installed
pacman -Q obs-studio

# If not:
sudo pacman -S obs-studio
```

### "ONNX Runtime library not found"

Make sure `ONNXRUNTIME_ROOT_DIR` points to the extracted directory:

```bash
# Check it exists
ls $ONNXRUNTIME_ROOT_DIR/lib/libonnxruntime.so

# If not found, re-export the path
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-linux-x64-1.19.2

# Rebuild
cd build
cmake .. -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR
make -j$(nproc)
```

### Plugin Not Appearing in OBS

**Check plugin is installed:**
```bash
ls -la ~/.config/obs-studio/plugins/obs-background-filter/bin/obs-background-filter.so
```

**Check dependencies:**
```bash
ldd ~/.config/obs-studio/plugins/obs-background-filter/bin/obs-background-filter.so
```

All libraries should show paths, not "not found".

**Launch OBS with debug output:**
```bash
obs --verbose
```

### Performance Issues

**GPU acceleration:**
```bash
# Check if NVIDIA GPU is being used
nvidia-smi

# If you have NVIDIA, install CUDA
sudo pacman -S cuda

# Rebuild ONNX Runtime with CUDA support (advanced)
```

**Lighter model:**
Download MediaPipe instead of U2-Net for faster performance on CPU.

## Building Without ONNX Runtime

For testing the plugin structure without ML:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
make install
```

The plugin will compile but inference will be disabled. Good for development/testing.

## Advanced: System-Wide Installation

If you want to install for all users:

```bash
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR \
  -DCMAKE_INSTALL_PREFIX=/usr/local

make -j$(nproc)
sudo make install
```

This installs to `/usr/local/.config/obs-studio/plugins/`.

## Development Build

For debugging:

```bash
mkdir build-debug && cd build-debug
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

make -j$(nproc)
make install

# Run with debugger
gdb --args obs
```

## Clean Rebuild

If something goes wrong:

```bash
# Remove build directory
rm -rf build

# Clean install
rm -rf ~/.config/obs-studio/plugins/obs-background-filter

# Rebuild from scratch
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR
make -j$(nproc)
make install
```

## Success Checklist

- [ ] Dependencies installed (`obs-studio`, `opencv`, etc.)
- [ ] ONNX Runtime downloaded and extracted
- [ ] Environment variable `ONNXRUNTIME_ROOT_DIR` set
- [ ] CMake configuration successful
- [ ] Build completed without errors
- [ ] Plugin installed to `~/.config/obs-studio/plugins/`
- [ ] Model file downloaded
- [ ] OBS launches without errors
- [ ] Filter appears in OBS filters list

## Getting Help

If you're still stuck:

1. Check OBS logs: `~/.config/obs-studio/logs/`
2. Run `obs --verbose` from terminal
3. Verify all dependencies: `ldd ~/.config/obs-studio/plugins/obs-background-filter/bin/obs-background-filter.so`
4. Open an issue on GitHub with full error output

## CachyOS-Specific Notes

CachyOS often uses optimized packages:
- Uses newer GCC with optimizations
- May have march-native builds
- Generally faster compilation

Everything should "just work" with the instructions above!

---

**Enjoy your AI background removal on CachyOS! 🎉**

