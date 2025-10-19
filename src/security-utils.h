#pragma once

#include <string>
#include <vector>

namespace Security {

/**
 * Calculate SHA-256 hash of a file
 * @param filepath Path to the file
 * @return Hex-encoded SHA-256 hash, or empty string on error
 */
std::string CalculateFileSHA256(const std::string &filepath);

/**
 * Verify file matches expected SHA-256 hash
 * @param filepath Path to the file
 * @param expected_hash Expected SHA-256 hash (hex-encoded)
 * @return true if hash matches, false otherwise
 */
bool VerifyFileChecksum(const std::string &filepath, const std::string &expected_hash);

/**
 * Validate that a file path is safe (no directory traversal)
 * @param filepath Path to validate
 * @param allowed_base_paths List of allowed base directories
 * @return true if path is safe, false otherwise
 */
bool ValidatePath(const std::string &filepath, 
                  const std::vector<std::string> &allowed_base_paths);

/**
 * Sanitize a file path to prevent directory traversal
 * @param filepath Path to sanitize
 * @return Sanitized path, or empty string if unsafe
 */
std::string SanitizePath(const std::string &filepath);

/**
 * Check if file is within allowed directory
 * @param filepath Path to check
 * @param allowed_dir Allowed base directory
 * @return true if file is within directory, false otherwise
 */
bool IsPathInDirectory(const std::string &filepath, const std::string &allowed_dir);

/**
 * Validate configuration values are within safe ranges
 * @param threshold Segmentation threshold (0.0-1.0)
 * @param blur_amount Blur kernel size
 * @param edge_smoothing Edge smoothing amount
 * @return true if all values are valid, false otherwise
 */
bool ValidateConfigValues(float threshold, int blur_amount, int edge_smoothing);

/**
 * Verify ONNX model file integrity before loading
 * @param model_path Path to ONNX model
 * @param expected_hash Expected SHA-256 (empty string to skip verification)
 * @return true if model is valid and safe to load
 */
bool VerifyModelIntegrity(const std::string &model_path, 
                          const std::string &expected_hash = "");

} // namespace Security

