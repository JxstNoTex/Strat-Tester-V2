#pragma once

namespace scripting
{
	void register_function(const char* name, void(*funcPtr)(game::scriptInstance_t inst));
	int load_script();
}