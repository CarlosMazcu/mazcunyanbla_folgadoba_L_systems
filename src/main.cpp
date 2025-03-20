#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <ctime>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"
#include "../include/TreeToObjConverter.hpp" // Incluir el nuevo conversor
#include <imgui.h>
#include <imgui-SFML.h>

#define INITIAL_ANGLE (25.0f)
#define INITIAL_LENGTH (10.0f)
#define INITIAL_WIDTH (2.0f)

void imguiWindow(LSystem& ls, bool& readyToGenerate, bool& ruleApplied, bool& export3D) {
    static std::string axiomInput;
    static bool axiomSet = false;
    static int selectedMode = -1;
    static bool setobj = false;
    static std::vector<char> availableAxioms = { 'F', 'X', 'Y', 'A', 'B' };
    static std::vector<std::string> guidedActions = {
       "Expandir ramas",
       "Anadir hojas",
       "Arbol mas denso",
       "Arbol mas curvo",
       "Ramas mas anchas",
       "Hacerlo mas caotico"
    };

    ImGui::SetNextWindowPos(ImVec2(800.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(400.0f, 600.0f));
    ImGui::Begin("L-System Builder", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (!axiomSet) {
        ImGui::Text("Select Starting Axiom:");

        for (char axiom : availableAxioms) {
            std::string buttonLabel = std::string(1, axiom);
            if (ImGui::Button(buttonLabel.c_str(), ImVec2(40, 30))) {
                axiomInput = buttonLabel;
                ls.setAxiom(axiomInput);
                axiomSet = true;
                selectedMode = -1;
                ruleApplied = false;
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
    else {
        if (ImGui::Button("Return"))
        {
            axiomSet = false;
            setobj = false;
        }
        ImGui::Text("Axiom: %s", axiomInput.c_str());

        // Selección de modo
        ImGui::Text("Choose Mode:");
        if (ImGui::Button("Manual", ImVec2(80, 30))) { selectedMode = 0; }
        ImGui::SameLine();
        if (ImGui::Button("Guided", ImVec2(80, 30))) { selectedMode = 1; }
        ImGui::SameLine();
        if (ImGui::Button("Automatic", ImVec2(100, 30))) { selectedMode = 2; }

        ImGui::Separator();


        if (selectedMode == 1) {
            int step = ls.getGuidedStep();
            char axiom = axiomInput[0]; // Obtener el axioma seleccionado

            if (step == 0) {
                ImGui::Text("Step 1: What should happen first?");
                if (axiom == 'F') {
                    if (ImGui::Button("Make trunk longer", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
                    ImGui::SameLine();
                    if (ImGui::Button("Create branches", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
                }
                else {
                    if (ImGui::Button("Expand structure", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
                    ImGui::SameLine();
                    if (ImGui::Button("Make more compact", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
                }
            }
            else if (step == 1) {
                ImGui::Text("Step 2: How should the branches grow?");
                if (ImGui::Button("More curved", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
                ImGui::SameLine();
                if (ImGui::Button("More straight", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
            }
            else if (step == 2) {
                ImGui::Text("Step 3: Add leaves?");
                if (ImGui::Button("Yes", ImVec2(200, 30))) { ls.nextGuidedStep(0); }
                ImGui::SameLine();
                if (ImGui::Button("No", ImVec2(200, 30))) { ls.nextGuidedStep(1); }
            }
            else {
                ImGui::Text("L-System rule complete!");
                ImGui::Text("Generated rule: %c -> %s", axiom, ls.getCurrent().c_str());

                if (!ruleApplied) {
                    if (ImGui::Button("Generate Tree")) {
                        readyToGenerate = true;
                        ruleApplied = true;
                        setobj = true;
                    }
                }
                else {
                    if (ImGui::Button("Restart Instructions")) {
                        ls.resetGuided();
                        ruleApplied = false;
                    }
                }
            }
        }

        if (selectedMode == 2) {
            if (ImGui::Button("Generate Random Tree")) {
                ls.generateRandomRules();
                readyToGenerate = true;
                setobj = true;
            }
        }

        // Sección para exportación 3D
        if (setobj) {
            ImGui::Separator();
            ImGui::Text("3D Export Options:");

            static char filename[128] = "../assets/tree3d.obj";
            static const char* default_filename = "../assets/tree3d.obj";
            ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));

            static int resolution = 8;
            ImGui::SliderInt("Cylinder Resolution", &resolution, 3, 16);

            static bool includeLeaves = true;
            ImGui::Checkbox("Include Leaves", &includeLeaves);

            static float leafSize = 0.5f;
            if (includeLeaves) {
                ImGui::SliderFloat("Leaf Size", &leafSize, 0.1f, 2.0f);
            }

            static float randomness3D = 1.0f;
            ImGui::SliderFloat("3D Branching Factor", &randomness3D, 0.0f, 3.0f);
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("0 = Flat tree, 1 = Natural 3D, 2-3 = Very volumetric 3D branching");
            }

            if (ImGui::Button("Export to OBJ")) {
                // Generar un nombre único basado en timestamp
                time_t now = time(0);
                struct tm tstruct;
                char time_buffer[80];

#ifdef _WIN32
                localtime_s(&tstruct, &now);
#else
                tstruct = *localtime(&now);
#endif

                strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &tstruct);

                // Construir el nombre de archivo único
                char unique_filename[256];

                // Extraer el nombre base sin timestamp previos
                std::string fname(filename);
                size_t pos = fname.find_last_of(".");

                // Si el usuario ha modificado el nombre predeterminado, verificamos si contiene timestamp previos
                if (strcmp(filename, default_filename) != 0) {
                    size_t timestamp_pos = fname.find("_202"); // Buscar patrones de timestamp (_YYYYMMDD_)

                    if (timestamp_pos != std::string::npos && timestamp_pos > 0 && pos != std::string::npos) {
                        // Si existe un timestamp previo, lo eliminamos
                        std::string clean_base = fname.substr(0, timestamp_pos);
                        std::string ext = fname.substr(pos);
                        sprintf(unique_filename, "%s_%s%s", clean_base.c_str(), time_buffer, ext.c_str());
                    }
                    else if (pos != std::string::npos) {
                        // Sin timestamp previo, pero con extensión
                        std::string base = fname.substr(0, pos);
                        std::string ext = fname.substr(pos);
                        sprintf(unique_filename, "%s_%s%s", base.c_str(), time_buffer, ext.c_str());
                    }
                    else {
                        // Sin timestamp previo y sin extensión
                        sprintf(unique_filename, "%s_%s.obj", fname.c_str(), time_buffer);
                    }
                }
                else {
                    // Es el nombre predeterminado, simplemente añadimos el timestamp
                    sprintf(unique_filename, "../assets/tree3d_%s.obj", time_buffer);
                }

                // Configurar el conversor 3D con el nombre único
                TreeToObjConverter converter(unique_filename, resolution);
                converter.setGenerateLeaves(includeLeaves);
                converter.setLeafSize(leafSize);
                converter.setRandomnessFactor(randomness3D);

                // Convertir la cadena L-system a OBJ
                if (converter.convertLSystemToObj(ls.getCurrent(), INITIAL_ANGLE, INITIAL_LENGTH, INITIAL_WIDTH)) {
                    converter.generateOBJ();
                    export3D = true;

                    // Guardar el nombre generado temporalmente, pero mostrar versión limpia en la interfaz
                    static char display_filename[256];
                    strcpy(display_filename, unique_filename);

                    // Actualizar el campo de texto con un nombre base limpio para la próxima exportación
                    std::string clean_name(unique_filename);
                    size_t timestamp_pos = clean_name.find("_202"); // Buscar patrón de timestamp
                    size_t ext_pos = clean_name.find_last_of(".");

                    if (timestamp_pos != std::string::npos && ext_pos != std::string::npos) {
                        // Quitar el timestamp pero conservar la extensión
                        std::string base = clean_name.substr(0, timestamp_pos);
                        std::string ext = clean_name.substr(ext_pos);
                        sprintf(filename, "%s%s", base.c_str(), ext.c_str());
                    }
                    else {
                        // Copiar tal cual (no debería ocurrir con esta lógica)
                        strcpy(filename, default_filename);
                    }

                    // Para mostrar el nombre completo generado
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "OBJ file exported successfully!");
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Saved to: %s", display_filename);
                }
            }
            else if (export3D) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "OBJ file exported successfully!");
            }
        }
    }

    ImGui::End();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 600), "L-System Interactive Builder");
    ImGui::SFML::Init(window);
    sf::View view = window.getDefaultView();
    sf::Clock deltaClock;

    LSystem lSystem;
    TreeRenderer treeRenderer(lSystem, window, INITIAL_ANGLE, INITIAL_LENGTH);

    bool readyToGenerate = false;
    bool ruleApplied = false;
    bool export3D = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (readyToGenerate) {
            lSystem.generate(5);
            readyToGenerate = false;
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear();
        imguiWindow(lSystem, readyToGenerate, ruleApplied, export3D);
        ImGui::SFML::Render(window);

        treeRenderer.draw();
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}