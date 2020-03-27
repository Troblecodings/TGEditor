#include "TGMain.h"
#include <TGEngine.hpp>
#include <gamecontent/PlayerController.hpp>
#include <gamecontent/Actor.hpp>
#include <pipeline/buffer/Texturebuffer.hpp>
#include <gamecontent/camera/2DCamera.hpp>
#include <io/Resource.hpp>
#include <io/Font.hpp>

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

	loadResourceFile("Resources/TGEditor.tgr");

	const char* chars[] = { "Test", "Test2"};
	tge::fnt::createStringActor(tge::fnt::fonts.data(), chars, 2);

	startTGEngine();
	return 0;
}