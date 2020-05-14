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
#include <iomanip>
#include <sstream>
#include <pipeline/CommandBuffer.hpp>
#include <pipeline/Draw.hpp>
#include <future>

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

    static std::string projectPath(RESOURCE_FOLDER);
    static std::string projectFolderPath;
    static COLORREF acrCustClr[16];

    inline static void reset() {
        tge::win::mouseHomogeneousX = 0;
        tge::win::mouseHomogeneousY = 0;
        tg_io::FIRST_MOUSE_BUTTON = false;
        flag = false;
    }

    inline static const char* openFileChooser(const char filter[], const uint64_t flags, bool* isMultiselect = nullptr, uint64_t* offset = nullptr) {
#ifdef WIN32
        char fileNames[2048];
        fileNames[0] = '\0';

        OPENFILENAMEA openFilenameInfo;
        ZeroMemory(&openFilenameInfo, sizeof(OPENFILENAMEA));
        openFilenameInfo.lStructSize = sizeof(OPENFILENAMEA);
        openFilenameInfo.lpstrFilter = filter;
        openFilenameInfo.nFilterIndex = 1;
        openFilenameInfo.lpstrInitialDir = tge::nio::current_working_dir;
        openFilenameInfo.lpstrFile = fileNames;
        openFilenameInfo.nMaxFile = sizeof(fileNames);
        openFilenameInfo.Flags = flags | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;

        if (GetOpenFileNameA(&openFilenameInfo)) {
            if (isMultiselect) {
                *isMultiselect = openFilenameInfo.nFileExtension == NULL;
            }
            if (offset) {
                *offset = openFilenameInfo.nFileOffset;
            }
            return fileNames;
        }
#endif // WIN32
        return nullptr;
    }

    inline static const uint32_t openColorChooser() {
#ifdef WIN32
        DWORD rgbCurrent;

        CHOOSECOLORA chooseColor;
        ZeroMemory(&chooseColor, sizeof(CHOOSECOLORA));
        chooseColor.lStructSize = sizeof(CHOOSECOLORA);
        chooseColor.Flags = CC_FULLOPEN;
        chooseColor.lpCustColors = acrCustClr;
        if (ChooseColorA(&chooseColor)) {
            // Transform from BGR to RGBA
            uint32_t color = chooseColor.rgbResult;
            color |= (color & 0xFF) << 24;
            uint32_t green = (color & 0xFF00) << 8;
            color &= 0xFFFF00FF;
            color |= (color & 0xFF0000) >> 8;
            color &= 0xFF00FFFF;
            color |= green;
            return  color | 0xFF;
        }
#endif // WIN32
        return 0xFFFFFFFF;
    }

    static uint32_t stringActorId = UINT32_MAX;
    static std::vector<std::string> stringsToWrite;
    static std::vector<glm::mat4> stringMatrix;

    inline static void recreateStrings() {
        std::thread recreationLoad([]() {
            std::ifstream input(projectPath);
            nlohmann::json json;
            input >> json;

            auto actorNames = json["actorNames"];
            auto materialNames = json["materialNames"];
            auto textureNames = json["textureNames"];

            const uint32_t reserveSize = actorNames.size() + materialNames.size() + textureNames.size();
            stringsToWrite.reserve(reserveSize);
            stringMatrix.reserve(reserveSize);

            constexpr float fontscale = 0.45f;

            uint32_t count = 0;
            for (const auto& actorname : actorNames) {
                std::string name = actorname.get<std::string>();
                stringsToWrite.push_back(name);
                stringMatrix.push_back(drw::genMatrix(-0.98f, -0.75f + count * 0.05, -0.5f, fontscale, fontscale));
                count++;
            }
            count = 0;
            for (const auto& materialname : materialNames) {
                std::string name = materialname.get<std::string>();
                stringsToWrite.push_back(name);
                stringMatrix.push_back(drw::genMatrix(-0.98f, -0.1f + count * 0.05, -0.5f, fontscale, fontscale));
                count++;
            }
            count = 0;
            for (const auto& texturename : textureNames) {
                std::string name = texturename.get<std::string>();
                stringsToWrite.push_back(name);
                stringMatrix.push_back(drw::genMatrix(-0.98f, 0.6f + count * 0.05, -0.5f, fontscale, fontscale));
                count++;
            }

            executionQueue.push_back([] {
                if (stringActorId != UINT32_MAX)
                    tge::fnt::destroyStrings(stringActorId);
                stringActorId = tge::fnt::createStringActor(tge::fnt::fonts.data(), stringsToWrite.data(), stringsToWrite.size(), stringMatrix.data());
                fillCommandBuffer();
            });
        });
        recreationLoad.detach();
    }

    void loadAdministration() noexcept {
        if (!fs::exists(RESOURCE_FOLDER))
            fs::create_directories(RESOURCE_FOLDER);

        projectPath.append(tgeproperties.getString("project"));

        while (projectPath.empty() || !fs::exists(projectPath) || projectPath.substr(projectPath.length() - 5).compare(".json") != 0) {
            projectPath = openFileChooser("Json (*.json)\0*.json\0\0", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST);
        }
        projectFolderPath = fs::path(projectPath).remove_filename().string();

        stringsToWrite.clear();
        stringMatrix.clear();
        recreateStrings();
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
        ui::boundingBoxFunctions.push_back([](uint32_t id) { });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { });

        // Add
        ui::boundingBoxFunctions.push_back([](uint32_t id) {
            if (!tg_io::FIRST_MOUSE_BUTTON || flag)
                return;
            flag = true;

            bool isMultiselect = false;
            uint64_t offset = 0;
            const char* returnstring = openFileChooser("Textures (*.png)\0*.png\0\0", OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, &isMultiselect, &offset);

            if (returnstring) {
                std::string command("texture add");
                if (isMultiselect) {
                    const char* fnames = returnstring + offset;
                    while (*fnames) {
                        command.append(" \"");
                        command.append(returnstring);
                        command.append("\\");
                        command.append(fnames);
                        command.append("\"");

                        const size_t len = strlen(fnames);
                        fnames += len + 1;
                    }
                } else {
                    command.append(" \"");
                    command.append(returnstring);
                    command.append("\"");
                }
                int CHECK(shadertool::exec(command.c_str()));
            }

            reset();
        });

        ui::boundingBoxFunctions.push_back([](uint32_t id) { 
            if (!tg_io::FIRST_MOUSE_BUTTON || flag)
                return;
            flag = true;

            uint64_t offset = 0;
            const char* file = openFileChooser("Textures (*.tgx)\0*.tgx\0\0", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, nullptr, &offset);
            if (file) {
                std::string command("material add ");

                const std::string filename = file + offset;
                const std::string fname = filename.substr(0, filename.length() - 4);
                command.append(fname);
                const uint32_t offset = command.length();

                command.append(" ");
                command.append(fname);

                std::stringstream stringstream;
                stringstream << std::hex << openColorChooser();
                std::string colorstring(stringstream.str());
                if (colorstring.length() < 8) {
                    const uint32_t fill = 8 - colorstring.length();
                    for (size_t i = 0; i <= fill; i++)
                        colorstring.insert(colorstring.begin(), '0');
                }
                command.append(" ");
                command.append(colorstring);

                uint8_t count = 0;

                std::string commandt = std::string(command);
                while (shadertool::exec(commandt.c_str()) != 0) {
                    commandt = std::string(command);
                    commandt.insert(offset, std::to_string(count)); 
                    count++;
                }
            }

            reset();
        });

        ui::boundingBoxFunctions.push_back([](uint32_t id) { 
            if (!tg_io::FIRST_MOUSE_BUTTON || flag)
                return;
            flag = true;

            std::thread materialRead([]() {
                std::string material(projectFolderPath);
                material.append("\\Materials.json");

                std::ifstream input(material);
                nlohmann::json json;
                input >> json;

                std::vector<std::string> materialNames;
                const uint32_t size = json.size();
                materialNames.reserve(size);
                for (auto& [key, value] : json.items()) {
                    auto str = key;
                    materialNames.push_back(str);
                }

                glm::mat4* transforms = new glm::mat4[size];

                uint32_t selected = 0;
                uint32_t* implselected = &selected;

                const uint32_t startoffset = ui::boundingBoxes.size();

                tge::ui::ListInputInfo inputInfo;
                inputInfo.scalefactor = 0.4f;
                inputInfo.heightOffset = tge::fnt::homogenHeight(tge::fnt::fonts.data());
                inputInfo.startX = 0;
                inputInfo.width = 0.1f;
                inputInfo.startY = 0;
                inputInfo.size = size;
                ui::createList(&inputInfo, 0.5f, 1, transforms, true, [=](uint32_t x) { 
                    if (!tg_io::FIRST_MOUSE_BUTTON)
                        return;
                    *implselected = x; 
                });
                const uint32_t text = tge::fnt::createStringActor(tge::fnt::fonts.data(), materialNames.data(), materialNames.size(), transforms);
                executionQueue.push_back(fillCommandBuffer);

                while (selected == 0);
                
                ui::deleteBoundingBoxes(startoffset, startoffset + size);
                tge::fnt::destroyStrings(text);

                std::string selection = materialNames[selected - startoffset];

                recreateStrings();
            });

            materialRead.detach();

            reset();
        });

        //Copy
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });
        ui::boundingBoxFunctions.push_back([](uint32_t id) { if (id == 0) printf("true"); });

    }
}