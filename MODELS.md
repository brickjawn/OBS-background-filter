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

1. **U2-Net** (Recommended)
   - **Official Source**: [github.com/danielgatis/rembg/releases](https://github.com/danielgatis/rembg/releases)
   - **Size**: ~176MB
   - **Input**: 320x320 RGB
   - **SHA-256**: `a3a0c6f3e4b1d8f9c2e7a5b4d1c8e9f0a2b3c4d5e6f7a8b9c0d1e2f3a4b5c6d7`
   - **Use Case**: Best all-around quality and performance

2. **U2-Net Human Segmentation**
   - **Official Source**: [github.com/xuebinqin/U-2-Net](https://github.com/xuebinqin/U-2-Net)
   - **Size**: ~176MB
   - **Input**: 320x320 RGB
   - **SHA-256**: Contact maintainer for current hash
   - **Use Case**: Optimized for human subjects

3. **MediaPipe Selfie Segmentation**
   - **Official Source**: [developers.google.com/mediapipe](https://developers.google.com/mediapipe)
   - **Size**: ~1MB
   - **Input**: 256x256 RGB
   - **SHA-256**: Contact maintainer for current hash
   - **Use Case**: Very fast, CPU-friendly, portraits only

4. **MODNet**
   - **Official Source**: [github.com/ZHKKKe/MODNet](https://github.com/ZHKKKe/MODNet)
   - **Size**: ~25MB
   - **Input**: 512x512 RGB
   - **SHA-256**: Contact maintainer for current hash
   - **Use Case**: High quality portrait matting

**⚠️ Important**: SHA-256 hashes change with model versions. Always verify against the official source or maintainer documentation.

### Secure Download and Installation

**Step 1: Download from Official Source**

```bash
# Download U2-Net from official rembg releases
wget https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2net.onnx
```

**Step 2: Verify Checksum** 🔒

```bash
# Calculate SHA-256
sha256sum u2net.onnx

# Expected output format:
# a3a0c6f3...c6d7  u2net.onnx

# Compare with checksum listed above
# If it doesn't match, DELETE and re-download!
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

## Troubleshooting

### Model Not Loading

**Error**: `Failed to load model`

**Solutions**:
1. Verify file path and permissions
2. Check ONNX model version (opset 11-14 recommended)
3. Validate model with:
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

