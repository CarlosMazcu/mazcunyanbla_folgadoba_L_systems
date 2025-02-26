#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <cmath>
#include "../include/LSystem.hpp"

#define ANGLE (25.0f) 
#define LENGTH (10.0f) 
#define M_PI (3.14159265358979323846264338327950288)

struct Point {
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
};

// Función para dibujar el árbol usando SFML
void drawTree(const std::string& lSystemString, sf::RenderWindow& window) {
    std::stack<Point> positionStack;
    std::stack<float> angleStack;
    Point currentPosition(400, 600); // Posición inicial en la pantalla
    float currentAngle = -90.0f; // Ángulo inicial (apuntando hacia arriba)

    for (char c : lSystemString) {
        switch (c) {
        case 'F': // Dibuja una línea hacia adelante
        {
            Point newPosition(
                currentPosition.x + LENGTH * cos(currentAngle * M_PI / 180.0f),
                currentPosition.y + LENGTH * sin(currentAngle * M_PI / 180.0f)
            );

            // Dibuja una línea usando SFML
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(currentPosition.x, currentPosition.y), sf::Color::Green),
                sf::Vertex(sf::Vector2f(newPosition.x, newPosition.y), sf::Color::Green)
            };
            window.draw(line, 2, sf::Lines);

            currentPosition = newPosition;
            break;
        }
        case '+': // Gira a la derecha
            currentAngle += ANGLE;
            break;
        case '-': // Gira a la izquierda
            currentAngle -= ANGLE;
            break;
        case '[': // Guarda la posición y el ángulo actual
            positionStack.push(currentPosition);
            angleStack.push(currentAngle);
            break;
        case ']': // Restaura la posición y el ángulo anterior
            currentPosition = positionStack.top();
            positionStack.pop();
            currentAngle = angleStack.top();
            angleStack.pop();
            break;
        }
    }
}

int main() {

    std::vector<LSystem::Axiom> axiomMap;

    LSystem::InitAxiomMap(axiomMap);

    int opcion;

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

    std::vector<std::pair<char, std::string>> reglas = { {seleccionado.letra, seleccionado.regla} };
    LSystem lSystem(std::string(1, seleccionado.letra), reglas);
    lSystem.generate(4);

    sf::RenderWindow window(sf::VideoMode(800, 600), "L-system Tree");

    sf::View view = window.getDefaultView();

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
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::G)
                    gKeyPressed = false;
                if (event.key.code == sf::Keyboard::R)
                    rKeyPressed = false;
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
        float moveSpeed = 1.0f * zoomFactor;

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !rKeyPressed) {
            lSystem.reset();
            lSystem.generate(1);
            rKeyPressed = true;
        }

        window.setView(view);
        window.clear();
        drawTree(lSystem.getCurrent(), window);
        window.display();
    }

    return 0;
}
