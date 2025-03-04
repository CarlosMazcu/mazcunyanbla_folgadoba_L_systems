#include <SFML/Graphics.hpp>
#include <iostream>
#include <set>
#include <stack>
#include <cmath>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"
#include <unordered_map>

#include <imgui.h>
#include <imgui-SFML.h>

#define INITIAL_ANGLE (25.0f)  // Ángulo inicial
#define INITIAL_LENGTH (10.0f) // Longitud inicial de la línea

void imguiWindow(std::unordered_map<std::string, std::unordered_map<char, std::string>>& rules, LSystem& ls) {
    static int selectedRuleIndex = -1;
    static std::string axiomInput;  // Cadena para el axioma de entrada
    static std::string selectedAxiom;  // Axioma seleccionado
    static std::string ruleString;
    static std::string productionString;  // Cadena para la producción de entrada
    static std::set<char> potentialAxioms;
    static bool clearAxioms = false;
    ImGui::SetNextWindowPos(ImVec2(800.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(400.0f, 600.0f));

    ImGui::Begin("Editor de Reglas", nullptr, ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse);

    //ImGui::Text("Axiom:");
    //ImGui::InputText("##Axiom", &axiomInput[0], 256);  // Campo para ingresar el axioma

    ImGui::Text("ABC of rules:");
    ImGui::Separator();

    // Definimos los caracteres de las reglas
    const std::vector<char> ruleChars = { 'F', 'f', '+', '-', '|', '[', ']', '!', '@', '{', '}', '>', '<', '&', '(', ')' };

    int columnCount = 4; // Número de botones por fila
    int counter = 0;

    for (char ruleChar : ruleChars) {
        if (ImGui::Button(std::string(1, ruleChar).c_str(), ImVec2(40, 30))) {
            productionString += ruleChar; // Agregar el símbolo a la producción actual
        }

        counter++;
        if (counter % columnCount != 0) {
            ImGui::SameLine();
        }
    }

    ImGui::InputText("##ProductionString", &productionString[0], 256);


    if (ImGui::Button("Analyze Rule")) {
        potentialAxioms.clear();
        for (char& ch : productionString) {
            if (isalpha(ch)) {
                potentialAxioms.insert(ch);
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
        productionString.erase();
    }

    if (!potentialAxioms.empty()) {
        ImGui::Text("Select Axiom:");
        for (char axiom : potentialAxioms) {
            std::string button_id = std::string(1, axiom) + "##" + std::to_string(axiom);
            if (ImGui::Button(&button_id[0], ImVec2(40, 30))) {
                axiomInput = std::string(1, axiom);
                // Verificar si ya existe una regla para este carácter bajo este axioma
                if (rules[axiomInput].find(axiom) != rules[axiomInput].end()) {
                    // Concatenar la nueva regla a la existente
                    rules[axiomInput][axiom] += "; " + productionString;
                }
                else {
                    // Crear una nueva entrada si no existe
                    rules[axiomInput][axiom] = productionString;
                }
                productionString.clear();
                selectedRuleIndex = -1;
                clearAxioms = true;
            }
            counter++;
            if (counter % columnCount != 0) {
                ImGui::SameLine();
            }
        }
        if (clearAxioms) {
            potentialAxioms.clear();
            clearAxioms = false;
        }
    }
    ImGui::Separator();
    ImGui::Text("Rules:");
    for (auto& axiom : rules) {
        for (auto& rule : axiom.second) {
            std::string ruleDisplay = axiom.first + " (" + rule.first + "): " + rule.second;
            // Usar el axioma como clave para la selección y mostrar cada regla en una nueva línea
            if (ImGui::Selectable(ruleDisplay.c_str(), selectedAxiom == axiom.first)) {
                selectedAxiom = axiom.first;  // Establecer el axioma seleccionado
            }
        }
    }

    // Opciones para la regla seleccionada
    if (!selectedAxiom.empty()) {
        ImGui::Text("Selected Axiom: %s", selectedAxiom.c_str());
        if (ImGui::Button("Delete Axiom")) {
            rules.erase(selectedAxiom);
            selectedAxiom.clear();  // Limpiar la selección
        }
    }

    ImGui::End();
}



int main() {
    std::unordered_map<std::string, std::unordered_map<char, std::string>> axiomRules;

    // Añadir axiomas y reglas
    axiomRules["F"] = { {'F', "FF+[+F-F-F]-[-F+F+F]"} };
    axiomRules["X"] = { {'X', "F[+X]F[-X]+X"}, {'F', "FF"} };
    int opcion;
    //std::vector<LSystem::Axiom> axiomMap;
    //LSystem::InitAxiomMap(axiomMap);

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
    //for (int i = 0; i < axiomMap.size(); i++)
    //{
    //    reglas.push_back({axiomMap[i].letra, axiomMap[i].regla});
    //}

    //std::vector<std::pair<char, std::string>> reglasARBOLCOMPUESTO = {
    //   {'A', "F[+A]F[-A]+A"},    // Ramificación compleja con recursividad
    //   {'F', "FF"}        // Regla para crecimiento de ramas
    //};
    std::string auxrule;
    // ESTO PARA EL ARBOL COMPUESTO
   /* LSystem lSystem("A", reglasARBOLCOMPUESTO);
    lSystem.generate(5);*/

    //ESTO PARA EL AXIOMMAP
    sf::RenderWindow window(sf::VideoMode(1200, 600), "L-System Complex Tree");
    ImGui::SFML::Init(window);
    sf::View view = window.getDefaultView();
    sf::Clock deltaClock;

    std::string ax = "F";
    LSystem lSystem(ax, axiomRules["F"]);
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
        
        imguiWindow(axiomRules, lSystem);
        
        ImGui::SFML::Render(window);
        
        treeRenderer.draw();
        window.display();
    }
    ImGui::SFML::Shutdown();

    return 0;
}
