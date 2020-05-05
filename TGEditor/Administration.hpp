#pragma once

#include <array>
#include <ui/UISystem.hpp>
#include "ShaderTool.hpp"
#include <io/Font.hpp>
#include <filesystem>
#include <json.hpp>
#include <pipeline/buffer/UniformBuffer.hpp>
#include <drawlib/Quad.hpp>

namespace fs = std::filesystem;
namespace drw = tge::drw;

using namespace tge::buf;

static bool flag = false;
static std::array<float, 784> uniformData{
 	1.0f, 0, 0, 0,
 	0, 1.0f, 0, 0,
 	0, 0, 0.1f, 0,
 	0, 0, 0, 1.0f
 };

static uint32_t currentSelectedActor = 0;
static std::vector<char*> actorNames;

constexpr char RESOURCE_FOLDER[] = "./Resources/";
constexpr char ACTOR_APPENDIX[] = "_Actor";
constexpr uint32_t ACTOR_APPENDIX_SIZE = sizeof(ACTOR_APPENDIX) / sizeof(*ACTOR_APPENDIX);

inline static void loadAdministration() noexcept {
    std::vector<std::string> stringsToWrite;
    std::vector<glm::mat4> stringMatrix;

    if (!fs::exists(RESOURCE_FOLDER))
        fs::create_directories(RESOURCE_FOLDER);

    fs::path projectPath(RESOURCE_FOLDER);
    projectPath.append("test.json");

    if (!fs::exists(projectPath)) {
        TGE_CRASH(L"The given project path doesn't exist!", TG_ERR_DB_FILE_NOT_FOUND);
    }

    std::ifstream input(projectPath);
    nlohmann::json json;
    input >> json;

    auto actorNames = json["actorNames"];
    auto materialNames = json["materialNames"];
    auto textureNames = json["textureNames"];

    stringsToWrite.reserve(actorNames.size() + materialNames.size() + textureNames.size());

    uint32_t count = 0;
    for (const auto& actorname : actorNames) {
        std::string name = actorname.get<std::string>();
        stringsToWrite.push_back(name);
        stringMatrix.push_back(drw::genMatrix(-0.98f, -0.75f + count * 0.05, -0.5f, 0.04f, 0.04f));
        count++;
    }
    count = 0;
    for (const auto& materialname : materialNames) {
        std::string name = materialname.get<std::string>();
        stringsToWrite.push_back(name);
        stringMatrix.push_back(drw::genMatrix(-0.98f, -0.1f + count * 0.05, -0.5f, 0.04f, 0.04f));
        count++;
    }
    count = 0;
    for (const auto& texturename : textureNames) {
        std::string name = texturename.get<std::string>();
        stringsToWrite.push_back(name);
        stringMatrix.push_back(drw::genMatrix(-0.98f, 0.6f + count * 0.05, -0.5f, 0.04f, 0.04f));
        count++;
    }

    const char** strings = new const char* [stringsToWrite.size()];
    for (size_t i = 0; i < stringsToWrite.size(); i++) {
        strings[i] = stringsToWrite[i].c_str();
    }
    tge::fnt::createStringActor(tge::fnt::fonts.data(), strings, stringsToWrite.size(), stringMatrix.data());
}

inline static void administration() noexcept {
    for (size_t i = 16; i < 16 + 128 * 4; i += 4) {
        uniformData[i + 2] = 1;
        uniformData[i + 3] = 1;
    }

    fillUniformBuffer(TRANSFORM_BUFFER, uniformData.data(), uniformData.size() * sizeof(float));
    fillUniformBuffer(TRANSFORM_BUFFER_2, uniformData.data(), uniformData.size() * sizeof(float));
}