# OBS Background Filter Plugin

An AI-powered background removal plugin for OBS Studio that uses deep learning to segment and remove or replace backgrounds in real-time video streams.

## ⚠️ Security Warning

**ONLY load ONNX models from trusted sources!**

Malicious model files can compromise your system. Always:
- ✅ Download from official sources only
- ✅ Verify SHA-256 checksums (see [MODELS.md](MODELS.md))
- ✅ Read our [Security Policy](SECURITY.md)
- ❌ Never use models from untrusted websites or users

**📖 See [SECURITY.md](SECURITY.md) for complete security guidelines.**

---

## Features

- **Real-time Background Removal**: Uses ML-based segmentation to identify and remove backgrounds
- **Multiple Background Options**:
  - Remove background (transparent)
  - Replace with solid color
  - Blur background
- **Edge Smoothing**: Advanced edge refinement for natural-looking results
- **GPU Acceleration**: Supports CUDA for NVIDIA GPUs
- **Adjustable Threshold**: Fine-tune detection sensitivity
- **Cross-platform**: Windows, Linux, and macOS support
- **🔒 Security Features**: SHA-256 verification, path validation, input sanitization

## Quick Start

### CachyOS/Arch Linux

```bash
# Install dependencies (Qt6 required for OBS 28.0+!)
sudo pacman -S base-devel cmake git pkgconf obs-studio opencv qt6-base openssl

# Download ONNX Runtime
wget https://github.com/microsoft/onnxruntime/releases/download/v1.19.2/onnxruntime-linux-x64-1.19.2.tgz
tar -xzf onnxruntime-linux-x64-1.19.2.tgz
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-linux-x64-1.19.2

# Clone and build
git clone https://github.com/brickjawn/OBS-background-filter.git
cd OBS-background-filter
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR
make -j$(nproc)
make install

# Download and verify model
mkdir -p ~/.config/obs-studio/plugins/obs-background-filter/data/models
wget https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2net.onnx \
  -O ~/.config/obs-studio/plugins/obs-background-filter/data/models/u2net.onnx
sha256sum ~/.config/obs-studio/plugins/obs-background-filter/data/models/u2net.onnx
# Verify checksum matches MODELS.md!
```

### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get install build-essential cmake git pkg-config
sudo apt-get install libobs-dev libopencv-dev qtbase5-dev libssl-dev

# Download ONNX Runtime
wget https://github.com/microsoft/onnxruntime/releases/download/v1.19.2/onnxruntime-linux-x64-1.19.2.tgz
tar -xzf onnxruntime-linux-x64-1.19.2.tgz
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-linux-x64-1.19.2

# Clone and build
git clone https://github.com/brickjawn/OBS-background-filter.git
cd OBS-background-filter
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DONNXRUNTIME_ROOT_DIR=$ONNXRUNTIME_ROOT_DIR
make -j$(nproc)
make install
```

## Usage

1. Launch OBS Studio
2. Right-click on a video source → **Filters**
3. Click **"+"** under "Effect Filters"  
4. Choose **"AI Background Removal"**
5. Configure settings:
   - **Threshold**: Adjust sensitivity (0.5 recommended)
   - **Replace Background**: Enable for solid color
   - **Blur Background**: Enable to blur instead
   - **Smooth Edges**: Enable for better quality

## Documentation

| Document | Purpose |
|----------|---------|
| [BUILDING.md](BUILDING.md) | Detailed build instructions for all platforms |
| [MODELS.md](MODELS.md) | Model selection, download, and safety guidelines |
| [SECURITY.md](SECURITY.md) | Security policy and best practices |
| [CONTRIBUTING.md](CONTRIBUTING.md) | How to contribute to the project |
| [CHANGELOG.md](CHANGELOG.md) | Version history and changes |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | Technical architecture and design |
| [docs/SECURITY_IMPLEMENTATION.md](docs/SECURITY_IMPLEMENTATION.md) | Security implementation details |

## System Requirements

### Minimum
- CPU: Intel Core i5 or equivalent
- RAM: 4 GB
- OBS Studio 28.0+
- Optional: GPU for acceleration

### Recommended
- CPU: Intel Core i7 or equivalent
- RAM: 8 GB
- GPU: NVIDIA GTX 1060 or better (CUDA support)
- OBS Studio 29.0+

## Configuration Examples

### Professional Video Calls
```
Threshold: 0.5
Replace Background: ✓
Replacement Color: Gray
Smooth Edges: ✓ (7)
```

### Streaming
```
Threshold: 0.4
Replace Background: ✓
Replacement Color: Green Screen
Smooth Edges: ✓ (5)
```

### Low-End Hardware
```
Threshold: 0.6
Smooth Edges: ✗
Use MediaPipe model
```

## Troubleshooting

### Plugin Not Appearing
- Check OBS logs: Help → Log Files → View Current Log
- Verify installation path: `~/.config/obs-studio/plugins/obs-background-filter/`
- Ensure dependencies installed: `ldd ~/.config/obs-studio/plugins/obs-background-filter/bin/obs-background-filter.so`

### Poor Performance
- Use lighter model (MediaPipe instead of U2-Net)
- Enable GPU acceleration
- Reduce OBS output resolution
- Lower threshold value

### Bad Segmentation
- Adjust threshold (try 0.3-0.7 range)
- Improve lighting
- Enable edge smoothing
- Try different model

## Security

This plugin implements production-grade security:
- ✅ SHA-256 checksum verification
- ✅ Path traversal protection (CVE-2025-51480)
- ✅ Input validation
- ✅ Model integrity checks
- ✅ Reduced ONNX attack surface

See [SECURITY.md](SECURITY.md) for complete details.

## Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Code of conduct
- Development setup
- Coding standards
- Pull request process

## License

GNU General Public License v2.0 - See [LICENSE](LICENSE) file.

## Credits

- **OBS Studio Team**: For the excellent streaming software
- **ONNX Runtime Team**: For the inference engine
- **OpenCV Community**: For computer vision tools
- **Security Researchers**: For responsible vulnerability disclosure

## Support

- **Issues**: [GitHub Issues](https://github.com/brickjawn/OBS-background-filter/issues)
- **Discussions**: [GitHub Discussions](https://github.com/brickjawn/OBS-background-filter/discussions)
- **Security**: See [SECURITY.md](SECURITY.md) for vulnerability reporting

---

**Status**: ✅ Production-ready with enterprise-grade security

**Version**: 1.0 with Security Enhancements

**Last Updated**: 2025-10-19
