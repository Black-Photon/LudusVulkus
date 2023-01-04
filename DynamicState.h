#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DynamicState {
public:
	std::vector<VkDynamicState> dynamic_states;

	DynamicState(bool viewport_dynamic, bool scissor_dynamic);

	bool is_viewport_dynamic();
	bool is_scissor_dynamic();

	void set_viewport_dynamic(bool val);
	void set_scissor_dynamic(bool val);

private:
	bool viewport_dynamic;
	bool scissor_dynamic;

	void regenerate_dynamic_states();
};

