#include "ShaderTool.hpp"
#include <iostream>
#include <string>

using namespace std;

namespace shadertool {

	int exec(const char* command) {
		char commandLineToRun[256];
		commandLineToRun[0] = '\0';
		strcat(commandLineToRun, SHADERTOOL_PATH);
		strcat(commandLineToRun, " ");
		strcat(commandLineToRun, command);
		return system(commandLineToRun);
	}
}