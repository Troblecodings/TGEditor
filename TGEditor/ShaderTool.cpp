#include "ShaderTool.hpp"
#include <iostream>
#include <string.h>
#include <io/Files.hpp>

namespace shadertool {

	int exec(const char* command) {
		// I am not proud of this
		constexpr char SHADERTOOL[] = "\\ShaderTool.exe ";
		std::string commandLineToRun(tge::nio::current_working_dir);
		commandLineToRun.append(SHADERTOOL);
		commandLineToRun.append(command);
		return std::system(commandLineToRun.c_str());
	}
}