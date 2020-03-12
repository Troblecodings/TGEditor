#include "TGMain.h"
#include <TGEngine.hpp>
#include <gamecontent/PlayerController.hpp>
#include <gamecontent/Actor.hpp>
#include <pipeline/buffer/Texturebuffer.hpp>
#include <gamecontent/camera/2DCamera.hpp>
#include <io/Resource.hpp>

using namespace tge::gmc;
using namespace tge::tex;

int main() {
	initEngine();

	playercontroller = [](Input input) {};

	Map map;
	loadResourceFile("test.tgr", &map);

	startTGEngine();
	return 0;
}