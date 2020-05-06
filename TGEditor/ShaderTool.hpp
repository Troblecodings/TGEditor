#pragma once

#define CHECK(rt) rtc = rt; std::cout << rtc << std::endl;

namespace shadertool {

	/*
	 * Represents the relative location off the ShaderTool executable
	 */
	constexpr char SHADERTOOL_PATH[] = "ShaderTool.exe";

	/*
	 * Is used to pass a command to the ShaderTool, just like one would enter a command into the ShaderTool itself
	 */
	int exec(const char* command);
}