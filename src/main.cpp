#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <cmath>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"


#define ANGLE (25.0f) 
#define LENGTH (10.0f) 

int main() {
    // Configuración del L-system
    std::string axiom = "X";
    std::vector<std::pair<char, std::string>> rules = {
        {'X', "F[+X][&-X]F[@]"}, // Rama principal con bifurcaciones, hojas y giros invertidos
        {'F', "F[<F]!F[#]"},      // Extensión de líneas con ramas secundarias más finas y efectos de ancho
        {'L', "{F+F+F+F}F"}       // Patrón de polígono (cuadrado) para simular flores o pequeñas hojas
    };

    // Crear el L-system
    LSystem lSystem(axiom, rules);
    lSystem.generate(6); // Número de iteraciones

    // Crear la ventana de SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "L-system Tree");

    // Crear el renderizador del árbol
    TreeRenderer treeRenderer(lSystem, window, ANGLE, LENGTH);

    // Bucle principal de la ventana
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        treeRenderer.draw(); // Dibujar el árbol
        window.display();
    }

    return 0;
}