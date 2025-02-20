#include "TreeRendered.hpp"

// Constantes para el ancho de l�nea
const float INITIAL_LINE_WIDTH = 2.0f;       // Ancho inicial de la l�nea
const float LINE_WIDTH_INCREMENT = 0.5f;    // Incremento/decremento del ancho de l�nea
const float MIN_LINE_WIDTH = 0.1f;          // Ancho m�nimo de la l�nea

// Constantes para la longitud de la l�nea
const float INITIAL_LENGTH = 10.0f;         // Longitud inicial de la l�nea
const float LENGTH_SCALE_FACTOR = 1.2f;     // Factor de escala para aumentar/reducir la longitud
const float MIN_LENGTH = 1.0f;              // Longitud m�nima de la l�nea

// Constantes para el �ngulo de giro
const float INITIAL_ANGLE = 25.0f;          // �ngulo inicial de giro (en grados)
const float ANGLE_INCREMENT_STEP = 5.0f;    // Paso para incrementar/disminuir el �ngulo de giro
const float MIN_ANGLE_INCREMENT = 1.0f;     // Valor m�nimo del incremento de �ngulo

// Otras constantes �tiles
const float PI = 3.1416f;                   // Valor de 

TreeRenderer::TreeRenderer(const LSystem& lSystem, sf::RenderWindow& window, float angle, float length)
    : lSystem(lSystem), window(window), ANGLE(angle), LENGTH(length) {}

void TreeRenderer::draw()
{
    drawTree(lSystem.getCurrent());
}

void TreeRenderer::drawTree(const std::string& lSystemString)
{
    std::stack<Point> positionStack; // Pila para guardar posiciones
    std::stack<float> angleStack;    // Pila para guardar �ngulos
    std::stack<float> lineWidthStack; // Pila para guardar el ancho de l�nea
    std::stack<float> lengthStack;   // Pila para guardar la longitud actual
    std::stack<float> angleIncrementStack; // Pila para guardar el incremento de �ngulo

    Point currentPosition(400, 600); // Posici�n inicial (centro de la ventana)
    float currentAngle = -90.0f;     // �ngulo inicial (apuntando hacia arriba)
    float lineWidth = INITIAL_LINE_WIDTH; // Ancho de l�nea inicial
    float currentLength = LENGTH;    // Longitud inicial de la l�nea
    float angleIncrement = ANGLE;    // Incremento de �ngulo inicial
    bool invertTurns = false;        // Bandera para invertir giros (+ y -)

    sf::VertexArray polygon(sf::Triangles); // Para construir pol�gonos
    bool inPolygon = false;                 // Indica si estamos dentro de un pol�gono
    sf::CircleShape point(lineWidth / 2.0f); // Para dibujar hojas

    // Recorrer la cadena del L-system
    for (char c : lSystemString) {
        switch (c) {
        case 'F': { // Dibujar una l�nea hacia adelante
            Point newPosition(
                currentPosition.x + currentLength * cos(currentAngle * PI / 180.0f),
                currentPosition.y + currentLength * sin(currentAngle * PI / 180.0f)
            );

            if (!inPolygon) {
                // Dibujar la l�nea usando SFML
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(currentPosition.x, currentPosition.y), sf::Color::Green),
                    sf::Vertex(sf::Vector2f(newPosition.x, newPosition.y), sf::Color::Green)
                };
                window.draw(line, 2, sf::Lines);
            }
            else {
                // Agregar v�rtices al pol�gono
                polygon.append(sf::Vertex(sf::Vector2f(currentPosition.x, currentPosition.y), sf::Color::Green));
            }

            currentPosition = newPosition; // Actualizar la posici�n actual
            break;
        }
        case 'f': { // Avanzar sin dibujar
            Point newPosition(
                currentPosition.x + currentLength * cos(currentAngle * PI / 180.0f),
                currentPosition.y + currentLength * sin(currentAngle * PI / 180.0f)
            );
            currentPosition = newPosition; // Actualizar la posici�n actual
            break;
        }
        case '+': // Girar a la izquierda
            if (!invertTurns) {
                currentAngle += angleIncrement;
            }
            else {
                currentAngle -= angleIncrement;
            }
            break;
        case '-': // Girar a la derecha
            if (!invertTurns) {
                currentAngle -= angleIncrement;
            }
            else {
                currentAngle += angleIncrement;
            }
            break;
        case '|': // Girar 180 grados
            currentAngle += 180.0f;
            break;
        case '[': // Guardar el estado actual
            positionStack.push(currentPosition);
            angleStack.push(currentAngle);
            lineWidthStack.push(lineWidth);
            lengthStack.push(currentLength);
            angleIncrementStack.push(angleIncrement);
            break;
        case ']': // Restaurar el estado anterior
            currentPosition = positionStack.top();
            positionStack.pop();
            currentAngle = angleStack.top();
            angleStack.pop();
            lineWidth = lineWidthStack.top();
            lineWidthStack.pop();
            currentLength = lengthStack.top();
            lengthStack.pop();
            angleIncrement = angleIncrementStack.top();
            angleIncrementStack.pop();
            break;
        case '#': // Incrementar el ancho de l�nea
            lineWidth += LINE_WIDTH_INCREMENT;
            break;
        case '!': // Disminuir el ancho de l�nea
            lineWidth -= LINE_WIDTH_INCREMENT;
            if (lineWidth < MIN_LINE_WIDTH) lineWidth = MIN_LINE_WIDTH;
            break;
        case '@': // Dibujar un punto (hoja)
            point.setRadius(lineWidth / 2.0f);
            point.setFillColor(sf::Color::Green);
            point.setPosition(currentPosition.x - lineWidth / 2.0f, currentPosition.y - lineWidth / 2.0f);
            window.draw(point);
            break;
        case '{': // Abrir un pol�gono
            inPolygon = true;
            polygon.clear();
            break;
        case '}': // Cerrar un pol�gono y rellenarlo
            if (inPolygon && polygon.getVertexCount() > 2) {
                window.draw(polygon);
            }
            inPolygon = false;
            break;
        case '>': // Multiplicar la longitud de la l�nea por el factor de escala
            currentLength *= LENGTH_SCALE_FACTOR;
            break;
        case '<': // Dividir la longitud de la l�nea por el factor de escala
            currentLength /= LENGTH_SCALE_FACTOR;
            if (currentLength < MIN_LENGTH) currentLength = MIN_LENGTH;
            break;
        case '&': // Intercambiar el significado de + y -
            invertTurns = !invertTurns;
            break;
        case '(': // Disminuir el �ngulo de giro
            angleIncrement -= ANGLE_INCREMENT_STEP;
            if (angleIncrement < MIN_ANGLE_INCREMENT) angleIncrement = MIN_ANGLE_INCREMENT;
            break;
        case ')': // Incrementar el �ngulo de giro
            angleIncrement += ANGLE_INCREMENT_STEP;
            break;
        }
    }
}