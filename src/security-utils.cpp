#include "security-utils.h"
#include <obs-module.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace Security {

std::string CalculateFileSHA256(const std::string &filepath)
{
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            blog(LOG_ERROR, "[Security] Failed to open file for hashing: %s", filepath.c_str());
            return "";
        }
        
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        
        constexpr size_t buffer_size = 8192;
        char buffer[buffer_size];
        
        while (file.read(buffer, buffer_size) || file.gcount() > 0) {
            SHA256_Update(&sha256, buffer, file.gcount());
        }
        
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_Final(hash, &sha256);
        
        // Convert to hex string
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') 
               << static_cast<int>(hash[i]);
        }
        
        return ss.str();
        
    } catch (const std::exception &e) {
        blog(LOG_ERROR, "[Security] Exception calculating SHA-256: %s", e.what());
        return "";
    }
}

bool VerifyFileChecksum(const std::string &filepath, const std::string &expected_hash)
{
    if (expected_hash.empty()) {
        blog(LOG_WARNING, "[Security] No checksum provided for verification");
        return true; // Skip verification if no hash provided
    }
    
    std::string calculated_hash = CalculateFileSHA256(filepath);
    if (calculated_hash.empty()) {
        return false;
    }
    
    // Case-insensitive comparison
    std::string expected_lower = expected_hash;
    std::string calculated_lower = calculated_hash;
    std::transform(expected_lower.begin(), expected_lower.end(), 
                   expected_lower.begin(), ::tolower);
    std::transform(calculated_lower.begin(), calculated_lower.end(), 
                   calculated_lower.begin(), ::tolower);
    
    if (expected_lower != calculated_lower) {
        blog(LOG_ERROR, "[Security] Checksum mismatch for %s", filepath.c_str());
        blog(LOG_ERROR, "[Security] Expected: %s", expected_hash.c_str());
        blog(LOG_ERROR, "[Security] Got:      %s", calculated_hash.c_str());
        return false;
    }
    
    blog(LOG_INFO, "[Security] Checksum verified for %s", filepath.c_str());
    return true;
}

bool ValidatePath(const std::string &filepath, 
                  const std::vector<std::string> &allowed_base_paths)
{
    try {
        // Check for directory traversal patterns
        if (filepath.find("..") != std::string::npos) {
            blog(LOG_ERROR, "[Security] Path contains '..' traversal: %s", filepath.c_str());
            return false;
        }
        
        // Convert to absolute path for comparison
        fs::path abs_path = fs::absolute(filepath);
        std::string abs_path_str = abs_path.string();
        
        // Check if path is within any allowed directory
        for (const auto &allowed_base : allowed_base_paths) {
            fs::path allowed_abs = fs::absolute(allowed_base);
            
            // Check if file path starts with allowed base path
            auto rel = fs::relative(abs_path, allowed_abs);
            if (!rel.empty() && rel.string().find("..") == std::string::npos) {
                blog(LOG_INFO, "[Security] Path validated: %s", filepath.c_str());
                return true;
            }
        }
        
        blog(LOG_ERROR, "[Security] Path not in allowed directories: %s", filepath.c_str());
        return false;
        
    } catch (const std::exception &e) {
        blog(LOG_ERROR, "[Security] Exception validating path: %s", e.what());
        return false;
    }
}

std::string SanitizePath(const std::string &filepath)
{
    try {
        // Remove any directory traversal attempts
        std::string sanitized = filepath;
        
        // Replace backslashes with forward slashes
        std::replace(sanitized.begin(), sanitized.end(), '\\', '/');
        
        // Remove ".." patterns
        while (sanitized.find("..") != std::string::npos) {
            size_t pos = sanitized.find("..");
            sanitized.erase(pos, 2);
        }
        
        // Remove multiple consecutive slashes
        while (sanitized.find("//") != std::string::npos) {
            size_t pos = sanitized.find("//");
            sanitized.erase(pos, 1);
        }
        
        // Normalize the path
        fs::path normalized = fs::path(sanitized).lexically_normal();
        
        return normalized.string();
        
    } catch (const std::exception &e) {
        blog(LOG_ERROR, "[Security] Exception sanitizing path: %s", e.what());
        return "";
    }
}

bool IsPathInDirectory(const std::string &filepath, const std::string &allowed_dir)
{
    try {
        fs::path file_abs = fs::absolute(filepath);
        fs::path dir_abs = fs::absolute(allowed_dir);
        
        // Get relative path
        auto rel = fs::relative(file_abs, dir_abs);
        
        // If relative path contains "..", file is outside directory
        return !rel.empty() && rel.string().find("..") == std::string::npos;
        
    } catch (const std::exception &e) {
        blog(LOG_ERROR, "[Security] Exception checking path: %s", e.what());
        return false;
    }
}

bool ValidateConfigValues(float threshold, int blur_amount, int edge_smoothing)
{
    // Validate threshold (0.0 to 1.0)
    if (threshold < 0.0f || threshold > 1.0f) {
        blog(LOG_ERROR, "[Security] Invalid threshold value: %f (must be 0.0-1.0)", threshold);
        return false;
    }
    
    // Validate blur amount (1 to 50)
    if (blur_amount < 1 || blur_amount > 50) {
        blog(LOG_ERROR, "[Security] Invalid blur_amount: %d (must be 1-50)", blur_amount);
        return false;
    }
    
    // Validate edge smoothing (1 to 10)
    if (edge_smoothing < 1 || edge_smoothing > 10) {
        blog(LOG_ERROR, "[Security] Invalid edge_smoothing: %d (must be 1-10)", edge_smoothing);
        return false;
    }
    
    return true;
}

bool VerifyModelIntegrity(const std::string &model_path, const std::string &expected_hash)
{
    // Check file exists
    if (!fs::exists(model_path)) {
        blog(LOG_ERROR, "[Security] Model file does not exist: %s", model_path.c_str());
        return false;
    }
    
    // Check file is regular file (not directory, symlink, etc.)
    if (!fs::is_regular_file(model_path)) {
        blog(LOG_ERROR, "[Security] Model path is not a regular file: %s", model_path.c_str());
        return false;
    }
    
    // Check file size is reasonable (< 500MB)
    auto file_size = fs::file_size(model_path);
    constexpr size_t max_size = 500 * 1024 * 1024; // 500 MB
    if (file_size > max_size) {
        blog(LOG_ERROR, "[Security] Model file too large: %zu bytes (max %zu)", 
             file_size, max_size);
        return false;
    }
    
    // Verify file extension is .onnx
    fs::path path_obj(model_path);
    if (path_obj.extension() != ".onnx") {
        blog(LOG_WARNING, "[Security] Model file does not have .onnx extension: %s", 
             model_path.c_str());
        // Warning only, not fatal
    }
    
    // Verify checksum if provided
    if (!expected_hash.empty()) {
        if (!VerifyFileChecksum(model_path, expected_hash)) {
            blog(LOG_ERROR, "[Security] Model checksum verification failed!");
            return false;
        }
    } else {
        blog(LOG_WARNING, "[Security] Loading model without checksum verification");
        blog(LOG_WARNING, "[Security] This is insecure! Provide checksums for production use.");
    }
    
    return true;
}

} // namespace Security

