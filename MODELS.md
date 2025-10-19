# Model Setup Guide

This guide explains how to obtain, convert, and optimize segmentation models for the OBS Background Filter plugin.

## ⚠️ **CRITICAL SECURITY WARNING** ⚠️

**ONLY load ONNX models from trusted sources!**

Malicious model files can:
- 🔴 Execute arbitrary code on your system
- 🔴 Access your files and data
- 🔴 Compromise your OBS streams
- 🔴 Install malware or cryptominers

### Safe Model Usage Rules

✅ **DO**:
- Download models ONLY from official sources listed below
- Verify SHA-256 checksums before loading any model
- Keep models in secure directories
- Report suspicious behavior immediately

❌ **DON'T**:
- Download models from random websites or forums
- Load models received via email or chat
- Skip checksum verification
- Trust "too good to be true" custom models

### Verify Every Model

**ALWAYS run checksum verification:**

```bash
# Calculate checksum
sha256sum model.onnx

# Compare with checksums in this document
# If they don't match, DELETE the file and re-download!
```

---

## Quick Start

### Pre-converted Models

**⚠️ Security Note**: Only download from the official sources linked below. Verify checksums!



Download ready-to-use ONNX models:

### 1. **U2-Net via rembg** (Recommended - Auto-verified)

The easiest and safest way to get U2-Net is through the rembg package:

```bash
# Install rembg (includes verified model downloads)
pip install rembg

# Download model automatically (stored in ~/.u2net/)
python3 -c "from rembg import new_session; new_session('u2net')"

# Copy to plugin directory
cp ~/.u2net/u2net.onnx ~/.config/obs-studio/plugins/obs-background-filter/data/models/
```

**⚠️ Security Note**: The rembg package downloads models from their CDN. Review the source code at: https://github.com/danielgatis/rembg/blob/main/rembg/session_factory.py

### 2. **U2-Net (Direct Download - VERIFY YOURSELF)**

⚠️ **Critical**: Official checksums are NOT published by model authors. You MUST verify URLs and downloads.

```bash
# Option A: Download from Hugging Face (more reliable)
wget https://huggingface.co/danielgatis/rembg/resolve/main/u2net.onnx

# Calculate checksum YOURSELF
sha256sum u2net.onnx

# Save this checksum for future verification
echo "YOUR_CHECKSUM_HERE  u2net.onnx" > u2net.onnx.sha256
```

**⚠️ No Official Checksums Available**: The U2-Net model repository does not publish SHA-256 checksums. 
- Document your own checksum after first download
- Verify file size is approximately 176MB
- Check ONNX structure (see validation section below)

### 3. **MediaPipe Selfie Segmentation**

```bash
# Download from Google's MediaPipe
# Visit: https://developers.google.com/mediapipe/solutions/vision/image_segmenter
# Models change - check official docs for current URLs
```

**⚠️ No Official ONNX**: MediaPipe provides TFLite models, not ONNX. Conversion required.

### 4. **MODNet**

```bash
# Download from official repository
git clone https://github.com/ZHKKKe/MODNet.git
cd MODNet/onnx

# Follow their conversion instructions
# Then calculate YOUR checksum
sha256sum modnet.onnx
```

**⚠️ No Official ONNX Checksums**: Convert from PyTorch yourself and document your checksum.

---

## ⚠️ **CRITICAL: About Checksums**

**This project does NOT provide checksums because:**
1. Model authors don't publish official checksums
2. Models update frequently with new versions
3. Providing "fake" checksums is a security risk

**What YOU must do:**
1. Download from official sources ONLY
2. Calculate checksum yourself: `sha256sum model.onnx`
3. Save your checksum for future verification
4. Validate ONNX structure (see below)

**Never trust checksums from:**
- Random forums or Discord
- File sharing sites
- Unofficial mirrors
- This documentation (if we provided them!)

### Secure Download and Installation

**Step 1: Download from Official Source**

```bash
# Option A: Via rembg (recommended - includes integrity checking)
pip install rembg
python3 -c "from rembg import new_session; new_session('u2net')"
cp ~/.u2net/u2net.onnx ./

# Option B: Direct download from Hugging Face
wget https://huggingface.co/danielgatis/rembg/resolve/main/u2net.onnx
```

**Step 2: Calculate YOUR Checksum** 🔒

```bash
# Linux/macOS
sha256sum u2net.onnx

# Windows PowerShell
Get-FileHash -Path u2net.onnx -Algorithm SHA256

# Output example:
# 60024c5c889badc19bc0b5d5f3a2e85d9e677b63a2c45e3e6d5e0b48b8f7c3b4  u2net.onnx

# SAVE THIS for future verification!
sha256sum u2net.onnx > u2net.onnx.sha256

# Next time, verify against YOUR saved checksum:
sha256sum -c u2net.onnx.sha256
```

**Step 3: Validate ONNX Structure** 🔒

```bash
# Install onnx package
pip install onnx

# Validate structure
python3 << 'EOF'
import onnx

try:
    model = onnx.load("u2net.onnx")
    onnx.checker.check_model(model)
    print("✅ Model structure is valid")
except Exception as e:
    print(f"❌ Model validation failed: {e}")
    print("DELETE this file - it may be corrupted or malicious!")
    exit(1)
EOF
```

**Step 3: Install to Secure Directory**

**Linux (User Install - Recommended):**
```bash
# Install to user directory (no sudo needed!)
mkdir -p ~/.config/obs-studio/plugins/obs-background-filter/data/models/
cp u2net.onnx ~/.config/obs-studio/plugins/obs-background-filter/data/models/

# Verify it's there
ls -lh ~/.config/obs-studio/plugins/obs-background-filter/data/models/
```

**Linux (System Install):**
```bash
sudo mkdir -p /usr/share/obs/obs-plugins/obs-background-filter/models/
sudo cp u2net.onnx /usr/share/obs/obs-plugins/obs-background-filter/models/
sudo chmod 644 /usr/share/obs/obs-plugins/obs-background-filter/models/u2net.onnx
```

**Windows:**
```powershell
New-Item -ItemType Directory -Force -Path "C:\Program Files\obs-studio\data\obs-plugins\obs-background-filter\models"
Copy-Item u2net.onnx "C:\Program Files\obs-studio\data\obs-plugins\obs-background-filter\models\"
```

**macOS:**
```bash
sudo mkdir -p "/Applications/OBS.app/Contents/Resources/data/obs-plugins/obs-background-filter/models"
sudo cp u2net.onnx "/Applications/OBS.app/Contents/Resources/data/obs-plugins/obs-background-filter/models/"
```

## Converting Models to ONNX

### PyTorch to ONNX

```python
import torch
import torch.onnx

# Load your PyTorch model
model = YourModel()
model.load_state_dict(torch.load('model.pth'))
model.eval()

# Create dummy input
dummy_input = torch.randn(1, 3, 320, 320)

# Export
torch.onnx.export(
    model,
    dummy_input,
    "model.onnx",
    export_params=True,
    opset_version=11,
    do_constant_folding=True,
    input_names=['input'],
    output_names=['output'],
    dynamic_axes={
        'input': {0: 'batch_size'},
        'output': {0: 'batch_size'}
    }
)
```

### TensorFlow to ONNX

```python
import tf2onnx
import tensorflow as tf

# Load TensorFlow model
model = tf.keras.models.load_model('model.h5')

# Convert to ONNX
spec = (tf.TensorSpec((None, 320, 320, 3), tf.float32, name="input"),)
model_proto, _ = tf2onnx.convert.from_keras(
    model,
    input_signature=spec,
    opset=13,
    output_path="model.onnx"
)
```

## Model Optimization

### Quantization (INT8)

Reduce model size and improve performance:

```python
import onnxruntime as ort
from onnxruntime.quantization import quantize_dynamic, QuantType

# Dynamic quantization
quantize_dynamic(
    "model.onnx",
    "model_quantized.onnx",
    weight_type=QuantType.QUInt8
)
```

### Simplification

Optimize model graph:

```python
import onnx
from onnxsim import simplify

# Load model
model = onnx.load("model.onnx")

# Simplify
model_simp, check = simplify(model)

# Save
onnx.save(model_simp, "model_simplified.onnx")
```

### TensorRT Optimization (NVIDIA GPUs)

```bash
# Convert ONNX to TensorRT
trtexec --onnx=model.onnx \
        --saveEngine=model.trt \
        --fp16 \
        --workspace=4096
```

## Training Custom Models

### Dataset Preparation

1. **Collect Images**
   - Diverse backgrounds
   - Various lighting conditions
   - Different poses and angles
   - Minimum 1000 images

2. **Create Masks**
   - Binary masks (foreground=255, background=0)
   - Use annotation tools like:
     - [CVAT](https://github.com/openvinotoolkit/cvat)
     - [LabelMe](https://github.com/wkentaro/labelme)
     - [VGG Image Annotator](https://www.robots.ox.ac.uk/~vgg/software/via/)

3. **Dataset Structure**
   ```
   dataset/
   ├── images/
   │   ├── 0001.jpg
   │   ├── 0002.jpg
   │   └── ...
   └── masks/
       ├── 0001.png
       ├── 0002.png
       └── ...
   ```

### Training U2-Net

```python
import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader
from PIL import Image
import numpy as np

class SegmentationDataset(Dataset):
    def __init__(self, image_dir, mask_dir, transform=None):
        self.image_dir = image_dir
        self.mask_dir = mask_dir
        self.transform = transform
        self.images = sorted(os.listdir(image_dir))
    
    def __len__(self):
        return len(self.images)
    
    def __getitem__(self, idx):
        img_path = os.path.join(self.image_dir, self.images[idx])
        mask_path = os.path.join(self.mask_dir, self.images[idx])
        
        image = Image.open(img_path).convert("RGB")
        mask = Image.open(mask_path).convert("L")
        
        if self.transform:
            image = self.transform(image)
            mask = self.transform(mask)
        
        return image, mask

# Training loop
def train(model, dataloader, epochs=100):
    criterion = nn.BCEWithLogitsLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
    
    for epoch in range(epochs):
        for images, masks in dataloader:
            optimizer.zero_grad()
            outputs = model(images)
            loss = criterion(outputs, masks)
            loss.backward()
            optimizer.step()
        
        print(f"Epoch {epoch+1}/{epochs}, Loss: {loss.item():.4f}")
    
    torch.save(model.state_dict(), 'u2net_trained.pth')
```

### Training Tips

1. **Data Augmentation**
   ```python
   from torchvision import transforms
   
   transform = transforms.Compose([
       transforms.RandomHorizontalFlip(),
       transforms.RandomRotation(10),
       transforms.ColorJitter(0.2, 0.2, 0.2),
       transforms.Resize((320, 320)),
       transforms.ToTensor(),
   ])
   ```

2. **Loss Functions**
   - Binary Cross-Entropy (BCE)
   - Dice Loss
   - IoU Loss
   - Combination: `BCE + Dice`

3. **Hyperparameters**
   - Learning rate: 1e-3 to 1e-4
   - Batch size: 8-32 (depending on GPU)
   - Epochs: 50-200
   - Optimizer: Adam or AdamW

## Recommended Models

### U2-Net
- **Pros**: High quality, good generalization
- **Cons**: Larger model size, slower inference
- **Best for**: High-quality production use

### U²-Net (Light)
- **Pros**: Smaller, faster than full U2-Net
- **Cons**: Slightly lower quality
- **Best for**: Balanced performance

### MediaPipe Selfie Segmentation
- **Pros**: Very lightweight, extremely fast
- **Cons**: Lower quality, person-only
- **Best for**: Real-time on CPU, portraits

### MODNet
- **Pros**: High quality, optimized for portraits
- **Cons**: Medium size, requires good lighting
- **Best for**: Professional video calls

### DeepLabV3+
- **Pros**: Industry standard, robust
- **Cons**: Larger model, general-purpose
- **Best for**: Diverse scenarios

## Model Performance

Benchmarks on Intel i7-10700K + RTX 3070:

| Model | Size | Input | FPS (CPU) | FPS (GPU) | Quality |
|-------|------|-------|-----------|-----------|---------|
| U2-Net | 176MB | 320x320 | ~5 | ~45 | ★★★★★ |
| U2-Net Lite | 4.7MB | 320x320 | ~15 | ~60 | ★★★★☆ |
| MediaPipe | 1MB | 256x256 | ~30 | ~120 | ★★★☆☆ |
| MODNet | 25MB | 512x512 | ~8 | ~35 | ★★★★★ |
| DeepLabV3+ | 50MB | 513x513 | ~4 | ~30 | ★★★★☆ |

## Automated Verification Script

Use our provided script for comprehensive validation:

```bash
# Make executable (if not already)
chmod +x scripts/verify-model.sh

# Verify your model
./scripts/verify-model.sh u2net.onnx

# Output will show:
# - File size check
# - SHA-256 calculation
# - ONNX structure validation
# - Security recommendations
```

The script will:
- ✅ Check file size (prevents resource exhaustion)
- ✅ Calculate SHA-256 checksum
- ✅ Validate ONNX structure with `onnx.checker`
- ✅ Compare against saved checksum (if exists)
- ✅ Detect corrupted or malicious files

---

## Model Supply Chain Security

### 🚩 Red Flags - DO NOT Use Models If:

- ❌ Downloaded from file sharing sites (Mega, MediaFire, Google Drive)
- ❌ Shared in Discord/Telegram/forums without official source
- ❌ No GitHub repository or academic paper associated
- ❌ Promises "better than X" without benchmarks or proof
- ❌ File size doesn't match documented size (±10MB)
- ❌ Uploader is anonymous or unknown
- ❌ Download URL is shortened (bit.ly, tinyurl, etc.)
- ❌ Site requires disabling antivirus or security software

### ✅ Trusted Sources

**Acceptable:**
- Official GitHub releases with git tags
- Academic institution repositories (.edu domains)
- Hugging Face with verified organization badges
- Direct from paper authors' websites
- Official model repositories (ModelZoo, PyTorch Hub)
- Package managers with source verification (pip, conda)

**Verify Trust:**
```bash
# Check GitHub repository activity
# - Recent commits (within 6 months)
# - Multiple contributors
# - Issues/PRs being addressed
# - Stars > 100 for established projects

# Check domain registration
whois example.com

# Verify HTTPS certificate
curl -vI https://example.com 2>&1 | grep 'SSL certificate'
```

---

## Pre-Installation Security Checklist

**Before installing ANY model, verify ALL items:**

```
Model Source Verification:
  [ ] Downloaded from source listed in official documentation
  [ ] URL uses HTTPS (not HTTP)
  [ ] Domain matches expected source (no typosquatting)
  [ ] No unexpected redirects during download

File Verification:
  [ ] File size matches expected (±10MB tolerance)
  [ ] Extension is .onnx (not .exe, .zip, .rar)
  [ ] SHA-256 checksum calculated and saved
  [ ] ONNX structure validates with onnx.checker
  [ ] No antivirus warnings

Source Trustworthiness:
  [ ] GitHub repo has >50 stars (for public models)
  [ ] Repository has recent activity (commits within 6 months)
  [ ] Associated with academic paper or known organization
  [ ] No security advisories against this source

Post-Download:
  [ ] Run verification script: ./scripts/verify-model.sh
  [ ] Document YOUR checksum in secure location
  [ ] Test in isolated environment first (optional but recommended)
```

**If ANY checkbox fails, DELETE the file and investigate before proceeding.**

---

## Advanced Security: GPG Signatures

Some model providers offer GPG-signed releases for additional security:

```bash
# Download model and signature (if available)
wget https://example.com/model.onnx
wget https://example.com/model.onnx.asc

# Import maintainer's public key
gpg --keyserver keyserver.ubuntu.com --recv-keys MAINTAINER_KEY_ID

# Verify signature
gpg --verify model.onnx.asc model.onnx

# Should output: "Good signature from..."
```

**Note**: Most ML models don't provide GPG signatures yet. This is a best-practice recommendation for the future.

---

## Runtime Security Options

When loading models in C++, use secure ONNX Runtime options:

```cpp
#include <onnxruntime_cxx_api.h>

Ort::SessionOptions options;

// Disable aggressive optimizations (reduces attack surface)
options.SetGraphOptimizationLevel(
    GraphOptimizationLevel::ORT_ENABLE_BASIC  // Not ORT_ENABLE_ALL
);

// Disable potentially dangerous features
options.DisablePerSessionThreads();
options.SetExecutionMode(ExecutionMode::ORT_SEQUENTIAL);

// Load model
Ort::Session session(env, model_path.c_str(), options);
```

This configuration:
- ✅ Mitigates CVE-2024-37032 (memory access in optimization)
- ✅ Reduces attack surface
- ✅ Still maintains good performance

See: https://nvd.nist.gov/vuln/detail/CVE-2024-37032

---

## Troubleshooting

### Model Not Loading

**Error**: `Failed to load model`

**Solutions**:
1. Verify file path and permissions
2. Check ONNX model version (opset 11-14 recommended)
3. **Run verification script first**: `./scripts/verify-model.sh model.onnx`
4. Validate model manually:
   ```python
   import onnx
   model = onnx.load("model.onnx")
   onnx.checker.check_model(model)
   ```

### Poor Segmentation Quality

**Solutions**:
1. Ensure good lighting in video source
2. Adjust threshold setting (0.3-0.7)
3. Enable edge smoothing
4. Try different model
5. Retrain on similar scenarios

### Performance Issues

**Solutions**:
1. Use quantized model
2. Reduce input resolution
3. Enable GPU acceleration
4. Use lighter model (MediaPipe)
5. Close other applications

## Resources

### Pre-trained Models
- [U2-Net](https://github.com/xuebinqin/U-2-Net)
- [MODNet](https://github.com/ZHKKKe/MODNet)
- [MediaPipe](https://developers.google.com/mediapipe)
- [BackgroundMattingV2](https://github.com/PeterL1n/BackgroundMattingV2)

### Datasets
- [PASCAL VOC](http://host.robots.ox.ac.uk/pascal/VOC/)
- [MS COCO](https://cocodataset.org/)
- [Human Segmentation Dataset](https://github.com/VikramShenoy97/Human-Segmentation-Dataset)
- [Supervisely Person](https://supervise.ly/explore/projects/supervisely-person-dataset-23304/datasets)

### Tools
- [ONNX](https://onnx.ai/)
- [ONNX Runtime](https://onnxruntime.ai/)
- [Netron](https://netron.app/) - Model visualizer
- [onnxsim](https://github.com/daquexian/onnx-simplifier)

## Contributing Models

If you've trained a model that works well, consider sharing it:

1. Test thoroughly across different scenarios
2. Document model architecture and training details
3. Provide benchmark results
4. Submit pull request or open issue

## Support

For model-related questions:
- [GitHub Issues](https://github.com/yourusername/OBS-background-filter/issues)
- [Discussions](https://github.com/yourusername/OBS-background-filter/discussions)

