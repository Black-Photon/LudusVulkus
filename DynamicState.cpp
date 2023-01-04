#include "DynamicState.h"

DynamicState::DynamicState(bool viewport_dynamic, bool scissor_dynamic) : viewport_dynamic(viewport_dynamic), scissor_dynamic(scissor_dynamic) {
	regenerate_dynamic_states();
}

bool DynamicState::is_viewport_dynamic() {
	return viewport_dynamic;
}

bool DynamicState::is_scissor_dynamic() {
	return scissor_dynamic;
}

void DynamicState::set_viewport_dynamic(bool val) {
	viewport_dynamic = val;
	regenerate_dynamic_states();
}

void DynamicState::set_scissor_dynamic(bool val) {
	scissor_dynamic = val;
	regenerate_dynamic_states();
}

void DynamicState::regenerate_dynamic_states() {
	dynamic_states.clear();
	if (viewport_dynamic) dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	if (scissor_dynamic) dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
}