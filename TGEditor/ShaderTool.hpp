#pragma once

namespace shadertool {

	/*
	 * Represents the relative location off the ShaderTool executable
	 */
	static const char* SHADERTOOL_PATH = "ShaderTool.exe";


	/*
	 * Is used to pass a command to the ShaderTool, just like one would enter a command into the ShaderTool itself
	 */
	int exec(const char* command);
}