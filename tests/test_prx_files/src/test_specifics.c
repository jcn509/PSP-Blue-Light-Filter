#include <pspdisplay.h>
#include <stdbool.h>

const int k_pixel_format = PIXEL_FORMAT;
#ifdef FILTER_ENABLED
const bool k_filter_enabled = true;
#else
const bool k_filter_enabled = false;
#endif