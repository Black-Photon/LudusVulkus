#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "Type.h"

std::vector<char> read_file(const std::string& filename);

std::string bool_str(bool b);