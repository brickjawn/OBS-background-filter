# Contributing to OBS Background Filter

Thank you for your interest in contributing! This document provides guidelines and information for contributors.

## Ways to Contribute

### 🐛 Bug Reports

Found a bug? Please create an issue with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- System information (OS, OBS version, GPU)
- Relevant logs from OBS

### 💡 Feature Requests

Have an idea? Open an issue describing:
- The feature and its benefits
- Use cases
- Possible implementation approaches
- Any relevant examples from other software

### 🔧 Code Contributions

We welcome pull requests! Please:
- Follow the coding standards below
- Add tests if applicable
- Update documentation
- Ensure builds pass on all platforms

### 📖 Documentation

Help improve:
- README and guides
- Code comments
- API documentation
- Tutorials and examples

### 🌍 Translations

Add support for more languages:
- Create locale files in `data/locale/`
- Follow existing `en-US.ini` format

## Development Setup

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git libobs-dev libopencv-dev

# Download ONNX Runtime
wget https://github.com/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-linux-x64-1.15.1.tgz
tar -xzf onnxruntime-linux-x64-1.15.1.tgz
export ONNXRUNTIME_ROOT_DIR=$(pwd)/onnxruntime-linux-x64-1.15.1
```

### Build for Development

```bash
# Clone your fork
git clone https://github.com/brickjawn/OBS-background-filter.git
cd OBS-background-filter

# Create debug build
mkdir build-debug && cd build-debug
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DONNXRUNTIME_ROOT_DIR=/path/to/onnxruntime \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

make -j$(nproc)

# Install to user directory for testing
mkdir -p ~/.config/obs-studio/plugins/obs-background-filter/bin
cp obs-background-filter.so ~/.config/obs-studio/plugins/obs-background-filter/bin/
```

### Testing Changes

```bash
# Run OBS from terminal to see debug output
obs

# Or run with GDB for debugging
gdb --args obs
```

## Coding Standards

### C++ Style

```cpp
// Use modern C++17 features
auto ptr = std::make_unique<Object>();

// Prefer const correctness
void ProcessFrame(const cv::Mat &input);

// Use RAII for resource management
class Filter {
    std::unique_ptr<Inference> inference_;  // Automatically cleaned up
};

// Clear naming conventions
int frame_width;              // Variables: snake_case
void ProcessVideoFrame();     // Functions: PascalCase
class BackgroundFilter {};    // Classes: PascalCase
const int MAX_THREADS = 4;    // Constants: UPPER_CASE
```

### Code Organization

```cpp
// Header files (.h)
#pragma once

#include <standard_library>
#include <third_party>
#include "local_headers.h"

// Forward declarations when possible
class Foo;

// Class definition
class Bar {
public:
    // Public interface first
    Bar();
    ~Bar();
    
    void PublicMethod();
    
private:
    // Private implementation details
    void PrivateMethod();
    int member_variable_;
};
```

### Error Handling

```cpp
// Check pointers before use
if (!frame || !filter) {
    blog(LOG_ERROR, "[Plugin] Null pointer detected");
    return false;
}

// Use try-catch for exceptions
try {
    model->RunInference(frame);
} catch (const std::exception &e) {
    blog(LOG_ERROR, "[Plugin] Inference failed: %s", e.what());
    return false;
}

// Log errors appropriately
blog(LOG_INFO, "Informational message");
blog(LOG_WARNING, "Warning message");
blog(LOG_ERROR, "Error message");
```

### Performance Considerations

```cpp
// Avoid unnecessary copies
void ProcessFrame(const cv::Mat &input);  // Pass by const reference

// Use move semantics when appropriate
std::vector<float> data = std::move(temp_data);

// Consider caching expensive operations
if (width != cached_width_ || height != cached_height_) {
    RecalculateBuffers();
    cached_width_ = width;
    cached_height_ = height;
}

// Profile before optimizing
// Use perf, valgrind, or nsys for profiling
```

## Git Workflow

### Branching Strategy

```bash
# Create feature branch from main
git checkout main
git pull origin main
git checkout -b feature/your-feature-name

# Make changes
git add .
git commit -m "Add feature description"

# Keep branch updated
git fetch origin
git rebase origin/main

# Push to your fork
git push origin feature/your-feature-name
```

### Commit Messages

Follow conventional commits:

```
feat: Add virtual background support
fix: Resolve memory leak in inference engine
docs: Update building instructions for macOS
perf: Optimize frame processing pipeline
refactor: Simplify model loading logic
test: Add unit tests for preprocessing
chore: Update dependencies
```

Detailed commit message example:
```
feat: Add support for custom background images

- Implement image loading from file
- Add property for background image path
- Support common formats (PNG, JPG, WebP)
- Cache loaded images for performance

Closes #42
```

## Pull Request Process

### Before Submitting

- [ ] Code compiles without warnings
- [ ] Tested on Linux (bonus: Windows/macOS)
- [ ] No memory leaks (check with valgrind)
- [ ] Documentation updated
- [ ] Follows coding standards
- [ ] Commit messages are clear

### PR Template

```markdown
## Description
Brief description of changes

## Motivation
Why is this change needed?

## Changes
- List of changes
- With details

## Testing
How was this tested?

## Screenshots (if applicable)
Before/after comparisons

## Checklist
- [ ] Code compiles
- [ ] Tested functionality
- [ ] Documentation updated
- [ ] No performance regression
```

## Testing Guidelines

### Manual Testing

```bash
# Build and install
./scripts/build.sh
# Follow installation instructions

# Test in OBS
1. Add video source
2. Add AI Background Removal filter
3. Test all settings combinations
4. Monitor CPU/GPU usage
5. Check for memory leaks
6. Test different video formats/resolutions
```

### Performance Testing

```bash
# CPU profiling
perf record -g obs
perf report

# Memory profiling
valgrind --leak-check=full obs

# GPU profiling (NVIDIA)
nsys profile obs
```

## Documentation

### Code Documentation

```cpp
/**
 * Process a video frame and apply background removal.
 * 
 * @param input_frame The input frame in BGR format
 * @param output_mask The output segmentation mask (0.0-1.0)
 * @param threshold Confidence threshold for segmentation
 * @return true if processing succeeded, false otherwise
 */
bool RunInference(const cv::Mat &input_frame, 
                  cv::Mat &output_mask, 
                  float threshold);
```

### User Documentation

- Use clear, concise language
- Include examples
- Add screenshots/diagrams
- Test instructions on fresh system

## Adding New Models

### Model Requirements

```python
# Input: [1, 3, H, W] float32 tensor, normalized [0, 1]
# Output: [1, 1, H, W] float32 tensor, segmentation mask

# Export to ONNX
torch.onnx.export(
    model,
    dummy_input,
    "model.onnx",
    opset_version=11,
    input_names=['input'],
    output_names=['output']
)
```

### Integration Steps

1. Add model to `data/models/`
2. Update `MODELS.md` with details
3. Test with various inputs
4. Document preprocessing requirements
5. Add to README

## Performance Optimization

### Profiling Workflow

```bash
# 1. Profile current performance
perf record -g obs

# 2. Identify hotspots
perf report

# 3. Optimize critical sections
# Focus on:
# - Memory allocations
# - Unnecessary copies
# - Algorithm complexity

# 4. Measure improvement
# Compare before/after metrics
```

### Optimization Checklist

- [ ] Minimize memory allocations
- [ ] Use efficient data structures
- [ ] Cache computed values
- [ ] Parallelize when possible
- [ ] Profile before/after changes
- [ ] Test on target hardware

## Release Process

### Version Numbering

Follow Semantic Versioning (SemVer):
- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes

### Creating a Release

1. Update version in `CMakeLists.txt`
2. Update `CHANGELOG.md`
3. Create git tag: `git tag v1.0.0`
4. Build release packages
5. Create GitHub release
6. Announce on forums/social media

## Community Guidelines

### Code of Conduct

- Be respectful and inclusive
- Welcome newcomers
- Give constructive feedback
- Focus on what's best for the project
- Show empathy and kindness

### Communication Channels

- **GitHub Issues**: Bug reports, features
- **GitHub Discussions**: Questions, ideas
- **Pull Requests**: Code reviews
- **OBS Forum**: Community support

## Getting Help

### Stuck on Something?

1. Check existing documentation
2. Search closed issues/PRs
3. Ask in GitHub Discussions
4. Reach out to maintainers

### Resources

- [OBS Plugin API](https://obsproject.com/docs/plugins.html)
- [ONNX Runtime Docs](https://onnxruntime.ai/docs/)
- [OpenCV Documentation](https://docs.opencv.org/)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)

## Recognition

Contributors will be:
- Listed in project README
- Credited in release notes
- Appreciated by the community! 🎉

## Questions?

Don't hesitate to ask! Open a discussion or issue, and we'll help you get started.

---

Thank you for contributing to OBS Background Filter! 🚀

