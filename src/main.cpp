#include <SFML/Graphics.hpp>
#include <iostream>
#include <stack>
#include <cmath>
#include "../include/LSystem.hpp"
#include "../include/TreeRendered.hpp"


#define ANGLE (25.0f) 
#define LENGTH (10.0f) 

int main() {
    // Configuraci�n del L-system
    std::string axiom = "X";
    std::vector<std::pair<char, std::string>> rules = {
        {'X', "F[+X][&-X]F[@]"}, // Rama principal con bifurcaciones, hojas y giros invertidos
        {'F', "F[<F]!F[#]"},      // Extensi�n de l�neas con ramas secundarias m�s finas y efectos de ancho
        {'L', "{F+F+F+F}F"}       // Patr�n de pol�gono (cuadrado) para simular flores o peque�as hojas
    };

    // Crear el L-system
    LSystem lSystem(axiom, rules);
    lSystem.generate(6); // N�mero de iteraciones

    // Crear la ventana de SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "L-system Tree");

    // Crear el renderizador del �rbol
    TreeRenderer treeRenderer(lSystem, window, ANGLE, LENGTH);

    // Bucle principal de la ventana
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        treeRenderer.draw(); // Dibujar el �rbol
        window.display();
    }

    return 0;
}