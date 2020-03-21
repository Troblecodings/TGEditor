#include "TGMain.h"
#include <TGEngine.hpp>
#include <gamecontent/PlayerController.hpp>
#include <gamecontent/Actor.hpp>
#include <pipeline/buffer/Texturebuffer.hpp>
#include <gamecontent/camera/2DCamera.hpp>
#include <io/Resource.hpp>

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

	startTGEngine();
	return 0;
}