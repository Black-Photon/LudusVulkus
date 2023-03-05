#pragma once

class Settings {
public:
    bool use_validation_layers;
    bool close_on_non_fatal;

    bool layer_verbose_enable;
    bool layer_info_enable;
    bool layer_warn_enable;
    bool layer_error_enable;

    bool is_mobile;
};