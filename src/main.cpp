#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <cmath>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"

#define INITIAL_ANGLE (25.0f)  // Ángulo inicial
#define INITIAL_LENGTH (10.0f) // Longitud inicial de la línea

int main() {
    std::vector<std::pair<char, std::string>> reglas;
    int opcion;
    std::vector<LSystem::Axiom> axiomMap;
    LSystem::InitAxiomMap(axiomMap);

    std::cout << "Seleccione un axioma:\n";
    for (size_t i = 0; i < axiomMap.size(); ++i) {
        std::cout << i + 1 << ": " << axiomMap[i].letra << " -> " << axiomMap[i].regla << "\n";
    }
    std::cin >> opcion;

    if (opcion < 1 || opcion > axiomMap.size()) {
        std::cout << "Opción inválida.\n";
        return 1;
    }

    LSystem::Axiom seleccionado = axiomMap[opcion - 1];
    reglas = { {seleccionado.letra, seleccionado.regla} };

    std::vector<std::pair<char, std::string>> reglasARBOLCOMPUESTO = {
       {'A', "F[+A]F[-A]+A"},    // Ramificación compleja con recursividad
       {'F', "FF"}        // Regla para crecimiento de ramas
    };

    // ESTO PARA EL ARBOL COMPUESTO
   /* LSystem lSystem("A", reglasARBOLCOMPUESTO);
    lSystem.generate(5);*/

    //ESTO PARA EL AXIOMMAP
    LSystem lSystem(std::string(1, seleccionado.letra), reglas);
    lSystem.generate(4);

    sf::RenderWindow window(sf::VideoMode(800, 600), "L-System Complex Tree");
    sf::View view = window.getDefaultView();

    TreeRenderer treeRenderer(lSystem, window, INITIAL_ANGLE, INITIAL_LENGTH);

    sf::Vector2f lastMousePos;
    bool dragging = false;
    bool gKeyPressed = false;
    bool rKeyPressed = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
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
            lSystem.generate(4);
            rKeyPressed = true;
        }

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R) && rKeyPressed) {
            rKeyPressed = false;
        }

        window.setView(view);
        window.clear();
        treeRenderer.draw();
        window.display();
    }

    return 0;
}
