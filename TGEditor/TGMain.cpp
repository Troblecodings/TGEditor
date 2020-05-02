#include "TGMain.h"
#include "ShaderTool.hpp"
#include <TGEngine.hpp>
#include <gamecontent/PlayerController.hpp>
#include <gamecontent/Actor.hpp>
#include <pipeline/buffer/Texturebuffer.hpp>
#include <gamecontent/camera/2DCamera.hpp>
#include <io/Resource.hpp>
#include <io/Font.hpp>
#include <drawlib/Quad.hpp>
#include <array>
#include <pipeline/buffer/UniformBuffer.hpp>

using namespace tge::io;
using namespace tge::gmc;
using namespace tge::tex;
using namespace tge::buf;

static TopDownCamera camera;

bool flag = false;
std::array<float, 784> uniformData {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 0.1, 0,
	0, 0, 0, 1
};

static void contoller(Input in) {}

int main() {
	initEngine();

	playercontroller = contoller;

	camera.positionx = 0;
	camera.positiony = 0;

#ifdef DEBUG
	int returncode = shadertool::exec("map make TGEditor"); // Recompile before loading
	OUT_LV_DEBUG(returncode)
#endif // Only in debug mode

	loadResourceFile("Resources/TGEditor.tgr");

	for (size_t i = 16; i < 16 + 128 * 4; i += 4) {
		uniformData[i + 2] = 1;
		uniformData[i + 3] = 1;
	}

	fillUniformBuffer(TRANSFORM_BUFFER, uniformData.data(), uniformData.size() * sizeof(float));
	fillUniformBuffer(TRANSFORM_BUFFER_2, uniformData.data(), uniformData.size() * sizeof(float));

	const char* chars[] = { "TEXTURES", "MATERIALS", "ACTORS", 
	"Name", "Name", "Name",
	"File", "Edit", "Project", "Settings"
};
	glm::mat4 materials[] = {
		tge::drw::genMatrix(-0.97f, -0.88f, -0.5f, 0.05f, 0.07f),
		tge::drw::genMatrix(-0.97f, -0.23f, -0.5f, 0.05f, 0.07f),
		tge::drw::genMatrix(-0.97f, 0.47f, -0.5f, 0.05f, 0.07f),

		tge::drw::genMatrix(-0.97f, -0.795f, -0.5f, 0.025f, 0.04f),
	    tge::drw::genMatrix(-0.97f, -0.145f, -0.5f, 0.025f, 0.04f),
		tge::drw::genMatrix(-0.97f, 0.555f, -0.5f, 0.025f, 0.04f),

		tge::drw::genMatrix(-0.95f, -0.97f, -0.5f, 0.03f, 0.04f),
		tge::drw::genMatrix(-0.88f, -0.97f, -0.5f, 0.03f, 0.04f),
		tge::drw::genMatrix(-0.81f, -0.97f, -0.5f, 0.03f, 0.04f),
		tge::drw::genMatrix(-0.70f, -0.97f, -0.5f, 0.03f, 0.04f)
	};
	constexpr uint32_t size = sizeof(chars) / sizeof(*chars);
	tge::fnt::createStringActor(tge::fnt::fonts.data(), chars, size, materials);

	startTGEngine();

	return 0;
}