#include "background-filter.h"
#include "security-utils.h"
#include <opencv2/opencv.hpp>
#include <util/platform.h>
#include <util/threading.h>

const char *background_filter_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return "AI Background Removal";
}

void *background_filter_create(obs_data_t *settings, obs_source_t *source)
{
    auto *filter = new background_filter_data();
    filter->context = source;
    filter->width = 0;
    filter->height = 0;
    filter->model_loaded = false;
    filter->processing = false;
    filter->last_process_time = 0;
    
    // Initialize model inference
    filter->inference = std::make_unique<ModelInference>();
    
    // Try to load the model
    const char *model_path = obs_module_file("models/u2net.onnx");
    if (model_path && filter->inference->LoadModel(model_path)) {
        filter->model_loaded = true;
        blog(LOG_INFO, "[Background Filter] Model loaded successfully");
    } else {
        blog(LOG_WARNING, "[Background Filter] Failed to load model from: %s", 
             model_path ? model_path : "null");
    }
    bfree((void *)model_path);
    
    background_filter_update(filter, settings);
    
    return filter;
}

void background_filter_destroy(void *data)
{
    auto *filter = static_cast<background_filter_data *>(data);
    
    // Wait for any ongoing processing
    std::lock_guard<std::mutex> lock(filter->process_mutex);
    
    delete filter;
}

void background_filter_update(void *data, obs_data_t *settings)
{
    auto *filter = static_cast<background_filter_data *>(data);
    
    // Read settings
    float threshold = (float)obs_data_get_double(settings, "threshold");
    int blur_amount = (int)obs_data_get_int(settings, "blur_amount");
    int edge_smoothing = (int)obs_data_get_int(settings, "edge_smoothing");
    
    // ===== SECURITY: Validate configuration values =====
    if (!Security::ValidateConfigValues(threshold, blur_amount, edge_smoothing)) {
        blog(LOG_ERROR, "[Background Filter] Invalid configuration values detected!");
        blog(LOG_ERROR, "[Background Filter] Using safe defaults instead.");
        threshold = 0.5f;
        blur_amount = 15;
        edge_smoothing = 3;
    }
    
    // Apply validated settings
    filter->threshold = threshold;
    filter->blur_background = obs_data_get_bool(settings, "blur_background");
    filter->blur_amount = blur_amount;
    filter->replace_background = obs_data_get_bool(settings, "replace_background");
    filter->replacement_color = (uint32_t)obs_data_get_int(settings, "replacement_color");
    filter->smooth_edges = obs_data_get_bool(settings, "smooth_edges");
    filter->edge_smoothing = edge_smoothing;
}

obs_properties_t *background_filter_properties(void *data)
{
    UNUSED_PARAMETER(data);
    
    obs_properties_t *props = obs_properties_create();
    
    obs_properties_add_float_slider(props, "threshold", 
        "Threshold", 0.0, 1.0, 0.01);
    
    obs_properties_add_bool(props, "blur_background", 
        "Blur Background");
    
    obs_properties_add_int_slider(props, "blur_amount", 
        "Blur Amount", 1, 50, 1);
    
    obs_properties_add_bool(props, "replace_background", 
        "Replace Background");
    
    obs_properties_add_color(props, "replacement_color", 
        "Replacement Color");
    
    obs_properties_add_bool(props, "smooth_edges", 
        "Smooth Edges");
    
    obs_properties_add_int_slider(props, "edge_smoothing", 
        "Edge Smoothing", 1, 10, 1);
    
    return props;
}

void background_filter_defaults(obs_data_t *settings)
{
    obs_data_set_default_double(settings, "threshold", 0.5);
    obs_data_set_default_bool(settings, "blur_background", false);
    obs_data_set_default_int(settings, "blur_amount", 15);
    obs_data_set_default_bool(settings, "replace_background", true);
    obs_data_set_default_int(settings, "replacement_color", 0xFF00FF00); // Green
    obs_data_set_default_bool(settings, "smooth_edges", true);
    obs_data_set_default_int(settings, "edge_smoothing", 3);
}

struct obs_source_frame *background_filter_video(void *data, struct obs_source_frame *frame)
{
    auto *filter = static_cast<background_filter_data *>(data);
    
    if (!filter->model_loaded || filter->processing) {
        return frame;
    }
    
    std::lock_guard<std::mutex> lock(filter->process_mutex);
    filter->processing = true;
    
    // Update dimensions if changed
    if (filter->width != frame->width || filter->height != frame->height) {
        filter->width = frame->width;
        filter->height = frame->height;
    }
    
    try {
        // Convert OBS frame to OpenCV Mat
        cv::Mat input_frame;
        
        if (frame->format == VIDEO_FORMAT_I420) {
            // Convert I420 to BGR
            cv::Mat yuv(frame->height + frame->height / 2, frame->width, CV_8UC1, frame->data[0]);
            cv::cvtColor(yuv, input_frame, cv::COLOR_YUV2BGR_I420);
        } else if (frame->format == VIDEO_FORMAT_NV12) {
            // Convert NV12 to BGR
            cv::Mat yuv(frame->height + frame->height / 2, frame->width, CV_8UC1, frame->data[0]);
            cv::cvtColor(yuv, input_frame, cv::COLOR_YUV2BGR_NV12);
        } else if (frame->format == VIDEO_FORMAT_RGBA) {
            input_frame = cv::Mat(frame->height, frame->width, CV_8UC4, frame->data[0]);
            cv::cvtColor(input_frame, input_frame, cv::COLOR_RGBA2BGR);
        } else {
            // Unsupported format
            filter->processing = false;
            return frame;
        }
        
        // Run inference to get mask
        cv::Mat mask;
        if (!filter->inference->RunInference(input_frame, mask, filter->threshold)) {
            filter->processing = false;
            return frame;
        }
        
        // Apply edge smoothing
        if (filter->smooth_edges && filter->edge_smoothing > 0) {
            cv::GaussianBlur(mask, mask, 
                cv::Size(filter->edge_smoothing * 2 + 1, filter->edge_smoothing * 2 + 1), 
                0);
        }
        
        // Process frame based on settings
        cv::Mat output_frame = input_frame.clone();
        
        if (filter->replace_background) {
            // Replace background with solid color
            cv::Vec3b bg_color(
                (filter->replacement_color >> 0) & 0xFF,   // B
                (filter->replacement_color >> 8) & 0xFF,   // G
                (filter->replacement_color >> 16) & 0xFF   // R
            );
            
            for (int y = 0; y < output_frame.rows; y++) {
                for (int x = 0; x < output_frame.cols; x++) {
                    float alpha = mask.at<float>(y, x);
                    output_frame.at<cv::Vec3b>(y, x) = 
                        output_frame.at<cv::Vec3b>(y, x) * alpha + 
                        bg_color * (1.0f - alpha);
                }
            }
        } else if (filter->blur_background) {
            // Blur background
            cv::Mat blurred;
            int kernel_size = filter->blur_amount * 2 + 1;
            cv::GaussianBlur(input_frame, blurred, cv::Size(kernel_size, kernel_size), 0);
            
            for (int y = 0; y < output_frame.rows; y++) {
                for (int x = 0; x < output_frame.cols; x++) {
                    float alpha = mask.at<float>(y, x);
                    output_frame.at<cv::Vec3b>(y, x) = 
                        input_frame.at<cv::Vec3b>(y, x) * alpha + 
                        blurred.at<cv::Vec3b>(y, x) * (1.0f - alpha);
                }
            }
        }
        
        // Convert back to original format
        if (frame->format == VIDEO_FORMAT_I420 || frame->format == VIDEO_FORMAT_NV12) {
            cv::Mat yuv_output;
            if (frame->format == VIDEO_FORMAT_I420) {
                cv::cvtColor(output_frame, yuv_output, cv::COLOR_BGR2YUV_I420);
            } else {
                cv::cvtColor(output_frame, yuv_output, cv::COLOR_BGR2YUV_NV12);
            }
            memcpy(frame->data[0], yuv_output.data, yuv_output.total() * yuv_output.elemSize());
        } else if (frame->format == VIDEO_FORMAT_RGBA) {
            cv::Mat rgba_output;
            cv::cvtColor(output_frame, rgba_output, cv::COLOR_BGR2RGBA);
            memcpy(frame->data[0], rgba_output.data, rgba_output.total() * rgba_output.elemSize());
        }
        
    } catch (const std::exception &e) {
        blog(LOG_ERROR, "[Background Filter] Error processing frame: %s", e.what());
    }
    
    filter->processing = false;
    return frame;
}

