#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <cmath>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#define INITIAL_ANGLE (25.0f)  // Ángulo inicial
#define INITIAL_LENGTH (10.0f) // Longitud inicial de la línea


void imguiWindow(std::vector<std::pair<char, std::string>>& rules, LSystem& ls) {
    static int selectedRuleIndex = -1;
    static std::string ruleString;
    ImGui::SetNextWindowPos(ImVec2(800.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(200.0f, 600.0f));

    ImGui::Begin("Editor de Reglas", nullptr, ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("ABC of rules:");
    ImGui::Separator();

    // Definimos los caracteres de las reglas
    const std::vector<char> ruleChars = { 'F', 'f', '+', '-', '|', '[', ']', '!', '@', '{', '}', '>', '<', '&', '(', ')' };

    int columnCount = 4; // Número de botones por fila
    int counter = 0;

    for (char ruleChar : ruleChars) {
        if (ImGui::Button(std::string(1, ruleChar).c_str(), ImVec2(40, 30))) {
            ruleString += ruleChar; // Agregar el símbolo a la regla actual
        }

        counter++;
        if (counter % columnCount != 0) {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();

    // Campo de texto editable para la regla actual
    ImGui::InputText("##RuleString", &ruleString[0], ImGuiInputTextFlags_ReadOnly);

    // Botón para agregar la regla a la lista
    if (ImGui::Button("Add Rule")) {
        if (!ruleString.empty()) {
            rules.emplace_back(ruleString[0], ruleString.substr(1)); // Guardar la regla
            ruleString.clear();
        }
    }

    ImGui::Separator();

    ImGui::Text("RULES:");

    for (int i = 0; i < rules.size(); i++) {
        std::string ruleText = std::string(1, rules[i].first) + " -> " + rules[i].second;

        if (ImGui::Selectable(ruleText.c_str(), selectedRuleIndex == i)) {
            selectedRuleIndex = i; // Guardar el índice de la regla seleccionada
        }
    }

    // Mostrar la regla seleccionada
    if (selectedRuleIndex >= 0 && selectedRuleIndex < rules.size()) {
        ImGui::Separator();
        ImGui::Text("Selected:");
        ImGui::Text("%c -> %s", rules[selectedRuleIndex].first, rules[selectedRuleIndex].second.c_str());
        ImGui::Separator();
        ImGui::SetCursorPosX((200.0f - 100.0f) * 0.5f);
        if (ImGui::Button("Run", ImVec2(100.0f, 30.0f)))
        {
            ls.reset();
        }
    }
    ImGui::End();
}


int main() {
    std::vector<std::pair<char, std::string>> reglas;
    int opcion;
    std::vector<LSystem::Axiom> axiomMap;
    LSystem::InitAxiomMap(axiomMap);

    //std::cout << "Seleccione un axioma:\n";
    //for (size_t i = 0; i < axiomMap.size(); ++i) {
    //    std::cout << i + 1 << ": " << axiomMap[i].letra << " -> " << axiomMap[i].regla << "\n";
    //}
    //std::cin >> opcion;

    //if (opcion < 1 || opcion > axiomMap.size()) {
    //    std::cout << "Opción inválida.\n";
    //    return 1;
    //}

    //LSystem::Axiom seleccionado = axiomMap[opcion - 1];
    for (int i = 0; i < axiomMap.size(); i++)
    {
        reglas.push_back({axiomMap[i].letra, axiomMap[i].regla});
    }

    //std::vector<std::pair<char, std::string>> reglasARBOLCOMPUESTO = {
    //   {'A', "F[+A]F[-A]+A"},    // Ramificación compleja con recursividad
    //   {'F', "FF"}        // Regla para crecimiento de ramas
    //};
    std::string auxrule;
    // ESTO PARA EL ARBOL COMPUESTO
   /* LSystem lSystem("A", reglasARBOLCOMPUESTO);
    lSystem.generate(5);*/

    //ESTO PARA EL AXIOMMAP
    sf::RenderWindow window(sf::VideoMode(1000, 600), "L-System Complex Tree");
    ImGui::SFML::Init(window);
    sf::View view = window.getDefaultView();
    sf::Clock deltaClock;

    LSystem lSystem(std::string(1, axiomMap[3].letra), reglas);
    lSystem.generate(lSystem.iterations);

    TreeRenderer treeRenderer(lSystem, window, INITIAL_ANGLE, INITIAL_LENGTH);

    sf::Vector2f lastMousePos;
    bool dragging = false;
    bool gKeyPressed = false;
    bool rKeyPressed = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta > 0)
                    view.zoom(0.9f);
                else
                    view.zoom(1.1f);
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                dragging = true;
                lastMousePos = sf::Vector2f(sf::Mouse::getPosition(window));
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                dragging = false;
            }
            else if (event.type == sf::Event::MouseMoved && dragging) {
                sf::Vector2f newMousePos = sf::Vector2f(sf::Mouse::getPosition(window));
                sf::Vector2f delta = lastMousePos - newMousePos;
                view.move(delta);
                lastMousePos = newMousePos;
            }
        }


        float zoomFactor = view.getSize().x / window.getDefaultView().getSize().x;
        float moveSpeed = 2.0f * zoomFactor;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            view.move(-moveSpeed, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            view.move(moveSpeed, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            view.move(0, -moveSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            view.move(0, moveSpeed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) && !gKeyPressed) {
            lSystem.generate(1);
            gKeyPressed = true;
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::G) && gKeyPressed) {
            gKeyPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !rKeyPressed) {
            lSystem.reset();
            lSystem.generate(lSystem.iterations);
            rKeyPressed = true;
        }

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R) && rKeyPressed) {
            rKeyPressed = false;
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        window.setView(view);
        window.clear();
        
        imguiWindow(reglas, lSystem);
        
        ImGui::SFML::Render(window);
        
        treeRenderer.draw();
        window.display();
    }
    ImGui::SFML::Shutdown();

    return 0;
}
