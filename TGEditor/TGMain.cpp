#include "ShaderTool.hpp"
#include <TGEngine.hpp>
#include <gamecontent/PlayerController.hpp>
#include <gamecontent/Actor.hpp>
#include <pipeline/buffer/Texturebuffer.hpp>
#include <gamecontent/camera/2DCamera.hpp>
#include <io/Resource.hpp>
#include <drawlib/Quad.hpp>
#include "Administration.hpp"
#include <io/Font.hpp>

using namespace tge::io;
using namespace tge::gmc;
using namespace tge::tex;
using namespace tge::buf;

static TopDownCamera camera;

static void contoller(Input in) {}

int main() {
	initEngine();

	playercontroller = contoller;

	camera.positionx = 0;
	camera.positiony = 0;

#ifdef DEBUG
	int returncode = shadertool::exec("map make TGEditor"); // Recompile before loading
	OUT_LV_DEBUG(returncode);
#endif // Only in debug mode

	loadResourceFile("Resources/TGEditor.tgr");

	const char* chars[] = {
		"TEXTURES", "MATERIALS", "ACTORS", "PROPERTIES",
		"Name", "Name", "Name",
		"File", "Edit", "Project", "Settings",
		"Example Label", "Example Label", "Example Label", "Example Label"
	};

	constexpr float SPACING = 0.03;
	constexpr float LABEL_SPACING = 0.085;

	glm::mat4 materials[] = {
		tge::drw::genMatrix(-1.0f + SPACING, -0.880f, -0.5f, 1, 1),
		tge::drw::genMatrix(-1.0f + SPACING, -0.230f, -0.5f, 1, 1),
		tge::drw::genMatrix(-1.0f + SPACING,  0.470f, -0.5f, 1, 1),
		tge::drw::genMatrix(0.5f + SPACING, -0.880f, -0.5f, 1, 1),

		tge::drw::genMatrix(-1.0f + SPACING, -0.795f, -0.5f, 0.5f, 0.5f),
		tge::drw::genMatrix(-1.0f + SPACING, -0.145f, -0.5f, 0.5f, 0.5f),
		tge::drw::genMatrix(-1.0f + SPACING,  0.555f, -0.5f, 0.5f, 0.5f),

		tge::drw::genMatrix(-0.95f, -0.970f, -0.5f, 0.4f, 0.4f),
		tge::drw::genMatrix(-0.88f, -0.970f, -0.5f, 0.4f, 0.4f),
		tge::drw::genMatrix(-0.81f, -0.970f, -0.5f, 0.4f, 0.4f),
		tge::drw::genMatrix(-0.70f, -0.970f, -0.5f, 0.4f, 0.4f),

		tge::drw::genMatrix(0.5f + SPACING * 1.5, -0.78f + LABEL_SPACING * 0, -0.5f, 0.4f, 0.4f),
		tge::drw::genMatrix(0.5f + SPACING * 1.5, -0.78f + LABEL_SPACING * 1, -0.5f, 0.4f, 0.4f),
		tge::drw::genMatrix(0.5f + SPACING * 1.5, -0.78f + LABEL_SPACING * 2, -0.5f, 0.4f, 0.4f),
		tge::drw::genMatrix(0.5f + SPACING * 1.5, -0.78f + LABEL_SPACING * 3, -0.5f, 0.4f, 0.4f),
	};
	constexpr uint32_t size = sizeof(chars) / sizeof(*chars);
	tge::fnt::createStringActor(tge::fnt::fonts.data(), chars, size, materials);


	// Must be last statement
	administration::loadAdministration();
	administration::administration();

	startTGEngine();

	return 0;
}