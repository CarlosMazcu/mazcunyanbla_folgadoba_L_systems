#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"
#include <imgui.h>
#include <imgui-SFML.h>

#define INITIAL_ANGLE (25.0f)
#define INITIAL_LENGTH (10.0f)

void imguiWindow(LSystem& ls, bool& readyToGenerate, bool& ruleApplied) {
    static std::string axiomInput;
    static bool axiomSet = false;
    static int selectedMode = -1; 
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
        if(ImGui::Button("Return"))
        { 
            axiomSet = false;
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
        imguiWindow(lSystem, readyToGenerate, ruleApplied);
        ImGui::SFML::Render(window);

        treeRenderer.draw();
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

