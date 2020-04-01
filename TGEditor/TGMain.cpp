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

using namespace tge::io;
using namespace tge::gmc;
using namespace tge::tex;

static TopDownCamera camera;

int main() {
	initEngine();

	camera.positionx = 0;
	camera.positiony = 0;
	playercontroller = [](Input input) {};

	setTopDownCamera(camera);

	shadertool::exec("map make TGEditor"); // Recompile before loading
	loadResourceFile("Resources/TGEditor.tgr");

	const char* chars[] = { "TEXTURES", "MATERIALS", "ACTORS" };
	glm::mat4 materials[] = {
		tge::drw::genMatrix(-0.97f, -0.85f, -0.5f, 0.08f, 0.08f),
		tge::drw::genMatrix(-0.97f, -0.2f, -0.5f, 0.08f, 0.08f),
		tge::drw::genMatrix(-0.97f, 0.5f, -0.5f, 0.08f, 0.08f)
	};
	tge::fnt::createStringActor(tge::fnt::fonts.data(), chars, 3, materials);

	startTGEngine();

	return 0;
}