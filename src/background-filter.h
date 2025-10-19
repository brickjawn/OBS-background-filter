#pragma once

#include <obs-module.h>
#include <memory>
#include "model-inference.h"

struct background_filter_data {
    obs_source_t *context;
    
    // Model inference engine
    std::unique_ptr<ModelInference> inference;
    
    // Filter settings
    float threshold;
    bool blur_background;
    int blur_amount;
    bool replace_background;
    uint32_t replacement_color;
    bool smooth_edges;
    int edge_smoothing;
    
    // Video format
    uint32_t width;
    uint32_t height;
    
    // Performance tracking
    uint64_t last_process_time;
    bool model_loaded;
    
    // Threading
    bool processing;
    std::mutex process_mutex;
};

// Plugin functions
extern "C" {
    const char *background_filter_get_name(void *unused);
    void *background_filter_create(obs_data_t *settings, obs_source_t *source);
    void background_filter_destroy(void *data);
    void background_filter_update(void *data, obs_data_t *settings);
    obs_properties_t *background_filter_properties(void *data);
    void background_filter_defaults(obs_data_t *settings);
    struct obs_source_frame *background_filter_video(void *data, struct obs_source_frame *frame);
}

