#ifndef __TREE_RENDERED_HPP__
#define __TREE_RENDERED_HPP__ 1


#include <SFML/Graphics.hpp>
#include <stack>
#include <cmath>
#include "LSystem.hpp"

class TreeRenderer {
public:
    // Constructor
    TreeRenderer(const LSystem& lSystem, sf::RenderWindow& window, float angle = 25.0f, float length = 10.0f);

    // Método para dibujar el árbol
    void draw();

private:
    // Referencias al L-system y a la ventana de SFML
    const LSystem& lSystem;
    sf::RenderWindow& window;

    // Parámetros configurables
    const float ANGLE;  // Ángulo de rotación
    const float LENGTH; // Longitud de cada segmento

    // Estructura para representar un punto en 2D
    struct Point {
        float x, y;
        Point(float x = 0, float y = 0) : x(x), y(y) {}
    };

    // Método privado para dibujar el árbol
    void drawTree(const std::string& lSystemString);
};


#endif // !__TREE_RENDERED_HPP__
