#include "TGMain.h"
#include <TGEngine.hpp>
#include <gamecontent/PlayerController.hpp>
#include <gamecontent/Actor.hpp>
#include <pipeline/buffer/Texturebuffer.hpp>
#include <gamecontent/camera/2DCamera.hpp>

using namespace tge::gmc;
using namespace tge::tex;

int main() {
	initEngine();

	playercontroller = [](Input* in){};

	TopDownCamera camera;
	camera.positionx = 0;
	camera.positiony = 0;
	camera.screenwidth = tge::win::mainWindowWidth;
	camera.screenheight = tge::win::mainWindowHeight;
	setTopDownCamera(&camera);

	SamplerInputInfo samplerInputInfo;
	samplerInputInfo.uSamplerMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInputInfo.vSamplerMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInputInfo.filterMagnification = VK_FILTER_NEAREST;
	samplerInputInfo.filterMignification = VK_FILTER_NEAREST;

	Sampler sampler;
	SamplerBindingInfo samplerbindinginfo;
	createSampler(samplerInputInfo, &sampler, &samplerbindinginfo);
	bindSampler(samplerbindinginfo, 0);
	bindSampler(samplerbindinginfo, 1);

	TextureInputInfo texture;
	texture.data = new uint8_t[4]{ 255, 255, 255, 255 };
	texture.x = 1;
	texture.y = 1;
	texture.comp = 1;

	Texture tex;
	TextureBindingInfo bindinginfo[2048];
	createTextures(&texture, 1, &tex, bindinginfo);
	bindTextures(bindinginfo, 2048, 0);
	bindTextures(bindinginfo, 2048, 1);

	createdMaterials = new Material[1];
	createdMaterials[0].color = { 1, 0, 0, 1};
	createdMaterials[0].diffuseTexture = 0;

	uint32_t actorIdx[] = { 0, 1, 2, 0, 2, 3 };
	float actorVertex[] = { -0.5, -0.5, 0, 0, 0.5, -0.5, 1, 0, 0.5, 0.5, 1, 1, -0.5, 0.5, 0, 1 };

	ActorInputInfo actor;
	actor.pProperties.localTransform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	actor.pProperties.material = 0;
	actor.pProperties.layer = 0;
	actor.indexCount = 6;
	actor.pIndices = actorIdx;
	actor.vertexCount = 4;
	actor.pVertices = (uint8_t*)actorVertex;
	createActor(&actor, 1);

	startTGEngine();
	return 0;
}