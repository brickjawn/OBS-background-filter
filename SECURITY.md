# Security Policy

## Overview

Security is a top priority for the OBS Background Filter plugin. This document outlines our security practices, known issues, and how to report vulnerabilities.

## 🔒 Security Features

### Implemented Protections

✅ **Path Validation** - Prevents directory traversal attacks (CVE-2025-51480 mitigation)
✅ **SHA-256 Checksum Verification** - Validates model file integrity
✅ **Configuration Validation** - Sanitizes all user input
✅ **Limited Graph Optimization** - Reduces ONNX Runtime attack surface
✅ **Whitelist-based Path Access** - Only allows models from trusted directories
✅ **File Type Validation** - Verifies ONNX file format
✅ **Size Limits** - Prevents resource exhaustion attacks
✅ **No Network Access** - Plugin operates entirely offline

### Architecture Security

- **Memory Safety**: Modern C++17 with RAII and smart pointers
- **No Arbitrary Code Execution**: Models run in ONNX Runtime sandbox
- **Minimal Privileges**: User-level installation (no root required)
- **Thread Safety**: Mutex-protected critical sections
- **Bounds Checking**: All array accesses validated

## ⚠️ Known Risks

###

 **CRITICAL: Untrusted Models**

**Risk Level**: 🔴 **CRITICAL**

Loading ONNX models from untrusted sources can lead to:
- Arbitrary code execution through ONNX Runtime vulnerabilities
- Path traversal attacks (CVE-2025-51480)
- Memory corruption exploits
- System compromise

**Mitigation**:
1. ✅ **ONLY** download models from official sources
2. ✅ **ALWAYS** verify SHA-256 checksums
3. ✅ Never load models from untrusted users or websites
4. ✅ Use models listed in our verified MODELS.md

### **ONNX Runtime Vulnerabilities**

**Known CVEs**:
- CVE-2024-37032: Memory access violation in graph optimization
- CVE-2025-51480: Path traversal in external data loading

**Our Mitigations**:
- Use `ORT_ENABLE_BASIC` optimization (not `ORT_ENABLE_ALL`)
- Path validation before model loading
- Regular dependency updates

### **OBS Plugin Permissions**

OBS plugins run with full access to:
- Video streams
- Audio streams
- System resources
- User files

**User Responsibility**:
- Only install plugins from trusted sources
- Verify plugin signatures when available
- Review source code before compiling

## 🛡️ Best Practices

### For Users

#### Safe Model Usage

```bash
# ✅ CORRECT: Download from official source with verification
wget https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2net.onnx
echo "EXPECTED_SHA256  u2net.onnx" | sha256sum -c

# ❌ WRONG: Download from untrusted source
# wget http://random-website.com/model.onnx  # DON'T DO THIS!
```

#### Verify Checksums

Always verify model checksums before use:

```bash
# Calculate checksum
sha256sum u2net.onnx

# Compare with known-good hash from MODELS.md
# If they don't match, DO NOT use the model!
```

#### Safe Installation

```bash
# ✅ CORRECT: Build from official repository
git clone https://github.com/brickjawn/OBS-background-filter.git
cd OBS-background-filter
# Verify git tags/signatures
./scripts/build.sh

# ❌ WRONG: Download random binary
# DO NOT download .so files from untrusted sources!
```

### For Developers

#### Code Review Checklist

When modifying security-critical code:

- [ ] Validate all file paths
- [ ] Check array bounds
- [ ] Sanitize user input
- [ ] Use const correctness
- [ ] Avoid raw pointers
- [ ] Check return values
- [ ] Log security events
- [ ] Update tests

#### Static Analysis

Run before committing:

```bash
# C++ static analysis
cppcheck --enable=all --suppress=missingIncludeSystem src/

# Clang-tidy
clang-tidy src/*.cpp -- -Isrc -I/usr/include/obs

# Address Sanitizer (during development)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
```

#### Secure Coding Standards

Follow these principles:

1. **Validate Input**: All external data is untrusted
2. **Fail Secure**: Default to safe behavior on error
3. **Least Privilege**: Request minimal permissions
4. **Defense in Depth**: Multiple layers of security
5. **Logging**: Record security-relevant events

## 🚨 Reporting Vulnerabilities

### How to Report

**DO NOT** open public issues for security vulnerabilities!

Instead, please:

1. **Email**: Send details to the project maintainer (see GitHub profile)
2. **PGP**: Encrypt sensitive information (key in repository)
3. **Details**: Include:
   - Vulnerability description
   - Steps to reproduce
   - Proof of concept (if applicable)
   - Suggested fix (if available)

### What to Expect

- **Acknowledgment**: Within 48 hours
- **Initial Assessment**: Within 1 week
- **Fix Timeline**: Depends on severity
  - Critical: 7 days
  - High: 14 days
  - Medium: 30 days
  - Low: 90 days
- **Public Disclosure**: After fix is released

### Responsible Disclosure

We follow responsible disclosure:

1. Reporter notifies us privately
2. We work on a fix
3. Fix is released
4. Public disclosure (with credit to reporter)

### Bug Bounty

Currently, we do not offer a bug bounty program. However, we will:
- Credit security researchers in release notes
- List contributors in SECURITY.md
- Provide recognition in the project

## 📋 Security Checklist

### Before Installing

- [ ] Downloaded from official GitHub repository
- [ ] Verified git commit signatures (if available)
- [ ] Reviewed source code (for security-conscious users)
- [ ] Built from source (not using untrusted binaries)

### Before Loading Models

- [ ] Model from official source only
- [ ] SHA-256 checksum verified
- [ ] Model file in trusted directory
- [ ] File size reasonable (<500MB)
- [ ] `.onnx` extension present

### During Use

- [ ] Monitor system resources (CPU, memory)
- [ ] Check OBS logs for security warnings
- [ ] Update plugin regularly
- [ ] Report suspicious behavior

## 🔄 Security Updates

### Update Policy

- **Security patches**: Released immediately
- **Dependency updates**: Monthly reviews
- **ONNX Runtime**: Updated within 7 days of security releases
- **OpenCV**: Updated with each major release

### How to Update

```bash
cd OBS-background-filter
git pull origin main
git verify-tag v1.0.1  # Verify tag signature
./scripts/build.sh
make install
```

### Notification Channels

- GitHub Security Advisories
- Release notes (CHANGES_APPLIED.md)
- OBS Forum security thread (if created)

## 📚 Security Resources

### Dependencies

Monitor these for security updates:

- [ONNX Runtime Security](https://github.com/microsoft/onnxruntime/security)
- [OpenCV Security](https://github.com/opencv/opencv/security)
- [OBS Studio Security](https://github.com/obsproject/obs-studio/security)

### CVE Databases

- [NIST NVD](https://nvd.nist.gov/)
- [MITRE CVE](https://cve.mitre.org/)
- [GitHub Security Advisories](https://github.com/advisories)

### Learning Resources

- [OWASP Secure Coding Practices](https://owasp.org/www-project-secure-coding-practices-quick-reference-guide/)
- [CWE Top 25](https://cwe.mitre.org/top25/)
- [CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682)

## 🏆 Hall of Fame

Security researchers who have helped improve this project:

- *Your name could be here!*

## 📜 Security Commitment

We commit to:

✅ Responding promptly to security reports
✅ Providing timely security updates  
✅ Being transparent about vulnerabilities
✅ Crediting security researchers
✅ Following industry best practices
✅ Continuous security improvement

## 📄 License

This security policy is part of the OBS Background Filter project, licensed under GPL v2.

---

**Last Updated**: 2025-10-19
**Policy Version**: 1.0

For questions about this security policy, open a GitHub Discussion or contact the maintainers directly.

**Remember**: Security is everyone's responsibility. If you see something, say something! 🔒

