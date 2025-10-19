#include <obs-module.h>
#include "background-filter.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-background-filter", "en-US")

MODULE_EXPORT const char *obs_module_description(void)
{
    return "AI-powered background removal filter for OBS";
}

bool obs_module_load(void)
{
    struct obs_source_info background_filter_info = {};
    
    background_filter_info.id = "background_removal_filter";
    background_filter_info.type = OBS_SOURCE_TYPE_FILTER;
    background_filter_info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_ASYNC;
    background_filter_info.get_name = background_filter_get_name;
    background_filter_info.create = background_filter_create;
    background_filter_info.destroy = background_filter_destroy;
    background_filter_info.update = background_filter_update;
    background_filter_info.get_properties = background_filter_properties;
    background_filter_info.get_defaults = background_filter_defaults;
    background_filter_info.filter_video = background_filter_video;
    
    obs_register_source(&background_filter_info);
    
    blog(LOG_INFO, "OBS Background Filter plugin loaded (version 1.0.0)");
    
    return true;
}

void obs_module_unload(void)
{
    blog(LOG_INFO, "OBS Background Filter plugin unloaded");
}

