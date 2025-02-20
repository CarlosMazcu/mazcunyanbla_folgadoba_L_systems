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

// Funci�n para dibujar el �rbol usando SFML
void drawTree(const std::string& lSystemString, sf::RenderWindow& window) {
    std::stack<Point> positionStack;
    std::stack<float> angleStack;
    Point currentPosition(400, 600); // Posici�n inicial en la pantalla
    float currentAngle = -90.0f; // �ngulo inicial (apuntando hacia arriba)

    for (char c : lSystemString) {
        switch (c) {
        case 'F': // Dibuja una l�nea hacia adelante
        {
            Point newPosition(
                currentPosition.x + LENGTH * cos(currentAngle * M_PI / 180.0f),
                currentPosition.y + LENGTH * sin(currentAngle * M_PI / 180.0f)
            );

            // Dibuja una l�nea usando SFML
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
        case '[': // Guarda la posici�n y el �ngulo actual
            positionStack.push(currentPosition);
            angleStack.push(currentAngle);
            break;
        case ']': // Restaura la posici�n y el �ngulo anterior
            currentPosition = positionStack.top();
            positionStack.pop();
            currentAngle = angleStack.top();
            angleStack.pop();
            break;
        }
    }
}

int main() {
    // Definimos el axioma y las reglas de producci�n
    std::string axiom = "F";
    std::vector<std::pair<char, std::string>> rules = {
        {'F', "F[+F][-F]"}
    };

    // Creamos el L-system
    LSystem lSystem(axiom, rules);

    // Generamos el �rbol despu�s de 4 iteraciones
    lSystem.generate(4);

    // Creamos una ventana de SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "L-system Tree");

    // Bucle principal de la ventana
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        drawTree(lSystem.getCurrent(), window); // Dibujamos el �rbol
        window.display();
    }

    return 0;
}