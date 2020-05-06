#include "Administration.hpp"

#include <ui/UISystem.hpp>
#include "ShaderTool.hpp"
#include <io/Font.hpp>
#include <filesystem>
#include <json.hpp>
#include <pipeline/buffer/UniformBuffer.hpp>
#include <drawlib/Quad.hpp>
#include <pipeline/window/Window.hpp>
#include <io/Mouse.hpp>

namespace administration {

    namespace fs = std::filesystem;
    namespace drw = tge::drw;
    namespace ui = tge::ui;

    using namespace tge::buf;

    std::atomic<bool> flag = false;
    std::array<std::atomic<float>, 784> uniformData{
        1.0f, 0, 0, 0,
        0, 1.0f, 0, 0,
        0, 0, 0.1f, 0,
        0, 0, 0, 1.0f
    };
    uint32_t currentSelectedActor = 0;
    std::vector<char*> actorNames;

    void loadAdministration() noexcept {
        std::vector<std::string> stringsToWrite;
        std::vector<glm::mat4> stringMatrix;

        if (!fs::exists(RESOURCE_FOLDER))
            fs::create_directories(RESOURCE_FOLDER);

        fs::path projectPath(RESOURCE_FOLDER);
        projectPath.append(tgeproperties.getString("project"));

        if (!fs::exists(projectPath)) {
            TGE_CRASH(L"The given project path doesn't exist! Please change the project name!", TG_ERR_DB_FILE_NOT_FOUND);
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
        delete strings;
    }

    void administration() noexcept {
        for (size_t i = 16; i < 16 + 128 * 4; i += 4) {
            uniformData[i + 2] = 1;
            uniformData[i + 3] = 1;
        }

        fillUniformBuffer(TRANSFORM_BUFFER, uniformData.data(), uniformData.size() * sizeof(float));
        fillUniformBuffer(TRANSFORM_BUFFER_2, uniformData.data(), uniformData.size() * sizeof(float));

        ui::boundingBoxes.reserve(ui::boundingBoxes.size() + 9);
        ui::boundingBoxFunctions.reserve(ui::boundingBoxFunctions.size() + 9);

        for (size_t i = 0; i < 3; i++) {
            ui::boundingBoxes.push_back({ -0.64f + 0.04f * i, -0.34f, -0.64f + 0.04f * i + 0.04f, -0.28f });
            ui::boundingBoxes.push_back({ -0.64f + 0.04f * i, 0.36f, -0.64f + 0.04f * i + 0.04f, 0.42f });
            ui::boundingBoxes.push_back({ -0.64f + 0.04f * i, 0.96f, -0.64f + 0.04f * i + 0.04f, 1.02f });
        }

        // Delete
        ui::boundingBoxFunctions.push_back([](uint32_t id) { });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });

        // Add
        ui::boundingBoxFunctions.push_back([](uint32_t id) {
            if (!tg_io::FIRST_MOUSE_BUTTON || flag)
                return;
            flag = true;
#ifdef WIN32
            char fileNames[2048];
            fileNames[0] = '\0';

            OPENFILENAMEA openfilenameinfo;
            ZeroMemory(&openfilenameinfo, sizeof(OPENFILENAMEA));
            openfilenameinfo.lStructSize = sizeof(OPENFILENAMEA);
            openfilenameinfo.lpstrFilter = "Textures (*.png)\0*.png\0\0";
            openfilenameinfo.nFilterIndex = 1;
            openfilenameinfo.lpstrFile = fileNames;
            openfilenameinfo.nMaxFile = sizeof(fileNames);
            openfilenameinfo.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
                | OFN_DONTADDTORECENT;

            if (GetOpenFileNameA(&openfilenameinfo)) {
                std::string command("texture add");
                if (openfilenameinfo.nFileExtension == NULL) {
                    char* fnames = openfilenameinfo.lpstrFile + openfilenameinfo.nFileOffset;
                    while (*fnames) {
                        command.append(" \"");
                        command.append(openfilenameinfo.lpstrFile);
                        command.append("\\");
                        command.append(fnames);
                        command.append("\"");

                        const size_t len = strlen(fnames);
                        fnames += len + 1;
                    }
                } else {
                    command.append(" \"");
                    command.append(openfilenameinfo.lpstrFile);
                    command.append("\"");
                }
                int CHECK(shadertool::exec(command.c_str()));
                CHECK(shadertool::exec(command.c_str()));
            }
          
            tge::win::mouseHomogeneousX = 0;
            tge::win::mouseHomogeneousY = 0;
            tg_io::FIRST_MOUSE_BUTTON = false;
            flag = false;
#endif
        });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });

        //Copy
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });

    }
}