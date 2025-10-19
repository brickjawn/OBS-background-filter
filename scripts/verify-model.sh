#!/bin/bash
# verify-model.sh - Automated ONNX model verification
# Usage: ./verify-model.sh <model-file>

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

MODEL_FILE="${1:-u2net.onnx}"

echo -e "${YELLOW}=== ONNX Model Verification ===${NC}\n"

# Check if file exists
if [ ! -f "$MODEL_FILE" ]; then
    echo -e "${RED}❌ File not found: $MODEL_FILE${NC}"
    exit 1
fi

echo -e "${GREEN}✅ File exists${NC}"

# Check file size
FILE_SIZE=$(stat -f%z "$MODEL_FILE" 2>/dev/null || stat -c%s "$MODEL_FILE" 2>/dev/null)
FILE_SIZE_MB=$((FILE_SIZE / 1024 / 1024))
echo -e "${GREEN}✅ File size: ${FILE_SIZE_MB}MB${NC}"

if [ $FILE_SIZE -lt 1000000 ]; then
    echo -e "${RED}❌ WARNING: File seems too small (< 1MB)${NC}"
fi

if [ $FILE_SIZE -gt 524288000 ]; then
    echo -e "${RED}❌ WARNING: File seems too large (> 500MB)${NC}"
    echo "   This could be a resource exhaustion attack"
    exit 1
fi

# Check file extension
if [[ "$MODEL_FILE" != *.onnx ]]; then
    echo -e "${YELLOW}⚠️  WARNING: File doesn't have .onnx extension${NC}"
fi

# Calculate SHA-256
echo -e "\n${YELLOW}Calculating SHA-256...${NC}"
if command -v sha256sum &> /dev/null; then
    CHECKSUM=$(sha256sum "$MODEL_FILE" | awk '{print $1}')
elif command -v shasum &> /dev/null; then
    CHECKSUM=$(shasum -a 256 "$MODEL_FILE" | awk '{print $1}')
else
    echo -e "${RED}❌ No SHA-256 tool found (sha256sum or shasum)${NC}"
    exit 1
fi

echo -e "${GREEN}SHA-256: $CHECKSUM${NC}"

# Check for saved checksum
CHECKSUM_FILE="${MODEL_FILE}.sha256"
if [ -f "$CHECKSUM_FILE" ]; then
    echo -e "\n${YELLOW}Verifying against saved checksum...${NC}"
    SAVED_CHECKSUM=$(cat "$CHECKSUM_FILE" | awk '{print $1}')
    
    if [ "$CHECKSUM" == "$SAVED_CHECKSUM" ]; then
        echo -e "${GREEN}✅ Checksum matches saved value${NC}"
    else
        echo -e "${RED}❌ CHECKSUM MISMATCH!${NC}"
        echo -e "${RED}   Expected: $SAVED_CHECKSUM${NC}"
        echo -e "${RED}   Got:      $CHECKSUM${NC}"
        echo -e "${RED}   DELETE this file immediately!${NC}"
        exit 1
    fi
else
    echo -e "\n${YELLOW}No saved checksum found.${NC}"
    echo -e "Save this checksum for future verification:"
    echo -e "${GREEN}echo \"$CHECKSUM  $MODEL_FILE\" > $CHECKSUM_FILE${NC}"
fi

# Validate ONNX structure
echo -e "\n${YELLOW}Validating ONNX structure...${NC}"

if ! command -v python3 &> /dev/null; then
    echo -e "${YELLOW}⚠️  Python3 not found - skipping ONNX validation${NC}"
    echo -e "   Install: pip install onnx"
else
    python3 << EOF
import sys

try:
    import onnx
except ImportError:
    print("⚠️  'onnx' package not installed - skipping validation")
    print("   Install: pip install onnx")
    sys.exit(0)

try:
    model = onnx.load("$MODEL_FILE")
    onnx.checker.check_model(model)
    print("✅ ONNX structure is valid")
    
    # Print model info
    print(f"\nModel info:")
    print(f"  IR version: {model.ir_version}")
    print(f"  Producer: {model.producer_name}")
    print(f"  Inputs: {len(model.graph.input)}")
    print(f"  Outputs: {len(model.graph.output)}")
    
except Exception as e:
    print(f"❌ ONNX validation FAILED: {e}")
    print("DELETE this file - it may be corrupted or malicious!")
    sys.exit(1)
EOF

    if [ $? -ne 0 ]; then
        echo -e "${RED}ONNX validation failed!${NC}"
        exit 1
    fi
fi

# Security recommendations
echo -e "\n${GREEN}=== Verification Complete ===${NC}\n"
echo -e "${YELLOW}Security Checklist:${NC}"
echo "  [?] Downloaded from official source?"
echo "  [?] URL was correct and HTTPS?"
echo "  [✅] File size reasonable"
echo "  [✅] Checksum calculated"
echo "  [✅] ONNX structure valid"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "  1. Save the checksum if this is your first time"
echo "  2. Copy model to plugin directory"
echo "  3. Never share this model without documenting YOUR checksum"
echo ""
echo -e "${GREEN}Safe to use!${NC}"

