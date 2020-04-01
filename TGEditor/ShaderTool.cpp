#include "ShaderTool.hpp"
#include <iostream>
#include <string.h>

namespace shadertool {

	int exec(const char* command) {
		char commandLineToRun[256];
		commandLineToRun[0] = '\0';
		std::strcat(commandLineToRun, SHADERTOOL_PATH);
		std::strcat(commandLineToRun, " ");
		std::strcat(commandLineToRun, command);
		return std::system(commandLineToRun);
	}
}