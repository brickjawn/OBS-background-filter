#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

// Forward declarations for ONNX Runtime
namespace Ort {
    class Env;
    class Session;
    class SessionOptions;
    struct Value;
    class MemoryInfo;
}

class ModelInference {
public:
    ModelInference();
    ~ModelInference();
    
    // Load ONNX model
    bool LoadModel(const std::string &model_path);
    
    // Run inference on a frame
    bool RunInference(const cv::Mat &input_frame, cv::Mat &output_mask, float threshold);
    
    // Check if model is loaded
    bool IsLoaded() const { return model_loaded_; }
    
    // Get model info
    void GetInputShape(int &height, int &width) const;
    
private:
    // Preprocess input image
    cv::Mat Preprocess(const cv::Mat &input);
    
    // Postprocess output to get mask
    cv::Mat Postprocess(float *output_data, int output_height, int output_width, 
                        int target_height, int target_width, float threshold);
    
    // ONNX Runtime components
    std::unique_ptr<Ort::Env> env_;
    std::unique_ptr<Ort::Session> session_;
    std::unique_ptr<Ort::SessionOptions> session_options_;
    std::unique_ptr<Ort::MemoryInfo> memory_info_;
    
    // Model configuration
    bool model_loaded_;
    int input_height_;
    int input_width_;
    std::vector<const char*> input_names_;
    std::vector<const char*> output_names_;
    std::vector<int64_t> input_shape_;
    std::vector<int64_t> output_shape_;
};

