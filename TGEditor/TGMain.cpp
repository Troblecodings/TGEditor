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
	OUT_LV_DEBUG(returncode)
#endif // Only in debug mode

	loadResourceFile("Resources/TGEditor.tgr");

	const char* chars[] = { "TEXTURES", "MATERIALS", "ACTORS", 
	"Name", "Name", "Name",
	"File", "Edit", "Project", "Settings"
};
	glm::mat4 materials[] = {
		tge::drw::genMatrix(-0.97f, -0.88f, -0.5f, 0.07f, 0.07f),
		tge::drw::genMatrix(-0.97f, -0.23f, -0.5f, 0.07f, 0.07f),
		tge::drw::genMatrix(-0.97f, 0.47f, -0.5f, 0.07f, 0.07f),

		tge::drw::genMatrix(-0.97f, -0.795f, -0.5f, 0.04f, 0.04f),
	    tge::drw::genMatrix(-0.97f, -0.145f, -0.5f, 0.04f, 0.04f),
		tge::drw::genMatrix(-0.97f, 0.555f, -0.5f, 0.04f, 0.04f),

		tge::drw::genMatrix(-0.95f, -0.97f, -0.5f, 0.04f, 0.04f),
		tge::drw::genMatrix(-0.88f, -0.97f, -0.5f, 0.04f, 0.04f),
		tge::drw::genMatrix(-0.81f, -0.97f, -0.5f, 0.04f, 0.04f),
		tge::drw::genMatrix(-0.70f, -0.97f, -0.5f, 0.04f, 0.04f)
	};
	constexpr uint32_t size = sizeof(chars) / sizeof(*chars);
	tge::fnt::createStringActor(tge::fnt::fonts.data(), chars, size, materials);


	// Must be last statement
	administration::loadAdministration();
	administration::administration();

	startTGEngine();

	return 0;
}