#include "../include/LSystem.hpp"
#include <ctime>
#include <iostream>

LSystem::LSystem() : originalAxiom(""), current("") {
    std::srand(std::time(nullptr)); 
}

void LSystem::setAxiom(const std::string& axiom) {
    originalAxiom = axiom;
    current = axiom;
    rules.clear(); 
}

void LSystem::addRule(char predecessor, const std::string& successor) {
    rules[predecessor] = successor;
}

void LSystem::iterate() {
    std::string newCurrent;
    for (char c : current) {
        auto rule = rules.find(c);
        if (rule != rules.end()) {
            newCurrent += rule->second;
        }
        else {
            newCurrent += c;
        }
    }
    current = newCurrent;
}

void LSystem::generate(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        iterate();
    }
}

void LSystem::reset() {
    current = originalAxiom;
}

std::string LSystem::getCurrent() const {
    return current;
}

void LSystem::generateGuidedRules(int action) {
    // Si aún no existe una regla para 'F', inicializarla con un simple avance
    if (rules.find('F') == rules.end()) {
        rules['F'] = "F";
    }

    std::string& currentRule = rules['F']; // Referencia a la regla existente

    switch (action) {
    case 0: // Expandir ramas
        currentRule += "[+F][-F]";
        break;
    case 1: // Añadir hojas
        currentRule += "[@]";
        break;
    case 2: // Árbol más denso
        currentRule += "F";
        break;
    case 3: // Árbol más curvo
        currentRule += "[+F]F[-F]";
        break;
    case 4: // Ramas más anchas
        currentRule += "[+F][-F]F";
        break;
    case 5: // Hacerlo más caótico
        currentRule += "F[-F][+F]F[-F]F";
        break;
    }

    // Reiniciar la cadena actual para reflejar los cambios acumulativos
    current = originalAxiom;
}



void LSystem::generateRandomRules() {
    rules.clear(); // Limpiar reglas previas antes de generar nuevas

    std::vector<std::string> F_rules = {
        "F[+F][-F]",
        "F[+F]F[-F]F",
        "FF+[+F-F-F]-[-F+F+F]",
        "F[+F]F[-F]F@",
        "F[-F][+F]F[-F]"
    };

    std::vector<std::string> X_rules = {
        "F[+X][-X]", // Crecimiento simétrico
        "F[+X]F[-X]+X", // Expande X con más crecimiento
        "X[-F][+F]FX", // Hace que X actúe como nodo intermedio
        "F[+X][-X]FXF@", // Le añade hojas (@)
        "X[+F][-F]F[@]" // Expande en nodos con hojas
    };

    std::vector<std::string> Y_rules = {
        "Y[+F]Y[-F]Y", // Similar a X pero más desordenado
        "F[+Y][-Y]FY", // Crecimiento más curvado
        "F[-Y]F[+Y]FY", // Expansión con balance
        "Y[-F]Y[+F]F", // Árbol más abierto
        "F[+Y]F[-Y]Y@" // Expansión con hojas en extremos
    };

    std::vector<std::string> A_rules = {
        "A[+A][-A]F", // Crecimiento fractal
        "A[+F][-F]F@", // Agrega hojas
        "F[+A]F[-A]F", // Crecimiento más ramificado
        "A[-F][+F]A", // Similar a X pero con más conexión
        "A[-F]A[+F]F@" // Crecimiento más denso
    };

    std::vector<std::string> B_rules = {
        "B[+B][-B]F", // Similar a A pero más disperso
        "B[+F][-F]F", // Patrón ramificado
        "F[+B]F[-B]F@", // Expansión con hojas
        "B[-F][+F]B", // Crecimiento más aleatorio
        "B[-F]B[+F]F@" // Similar a A pero más ancho
    };

    char axiom = originalAxiom[0]; // Obtener la letra del axioma seleccionado

    if (axiom == 'F') {
        rules[axiom] = F_rules[std::rand() % F_rules.size()];
    }
    else if (axiom == 'X') {
        rules[axiom] = X_rules[std::rand() % X_rules.size()];
        rules['F'] = "FF"; // X tiende a hacer que F crezca más
    }
    else if (axiom == 'Y') {
        rules[axiom] = Y_rules[std::rand() % Y_rules.size()];
        rules['F'] = "F[+F][-F]"; // Y tiende a hacer ramas más abiertas
    }
    else if (axiom == 'A') {
        rules[axiom] = A_rules[std::rand() % A_rules.size()];
        rules['F'] = "F[@]"; // A favorece la aparición de hojas
    }
    else if (axiom == 'B') {
        rules[axiom] = B_rules[std::rand() % B_rules.size()];
        rules['F'] = "FF[-F][+F]"; // B favorece estructuras más caóticas
    }

    // Reiniciar la cadena actual
    current = originalAxiom;
}

void LSystem::nextGuidedStep(int choice) {
    char axiom = originalAxiom[0]; // Obtener la letra del axioma seleccionado

    if (guidedStep == 0) {
        // Paso 1: Elección inicial según el axioma
        if (axiom == 'F') {
            rules['F'] = (choice == 0) ? "FF" : "F[+F][-F]";
        }
        else if (axiom == 'X') {
            rules['X'] = (choice == 0) ? "X[+F][-F]" : "X[+X][-X]";
        }
        else if (axiom == 'Y') {
            rules['Y'] = (choice == 0) ? "YF" : "Y[+Y][-Y]";
        }
        else if (axiom == 'A') {
            rules['A'] = (choice == 0) ? "AF" : "A[+A][-A]";
        }
        else if (axiom == 'B') {
            rules['B'] = (choice == 0) ? "B[-F][+F]" : "B[+B][-B]";
        }
    }
    else if (guidedStep == 1) {
        // Paso 2: Curvatura o rectitud de las ramas
        if (axiom == 'F') {
            rules['F'] += (choice == 0) ? "[+F]F[-F]" : "[+F][-F]F";
        }
        else if (axiom == 'X') {
            rules['X'] += (choice == 0) ? "[+X]X[-X]" : "[+X][-X]X";
        }
        else if (axiom == 'Y') {
            rules['Y'] += (choice == 0) ? "[+Y]Y[-Y]" : "Y[+Y][-Y]Y";
        }
        else if (axiom == 'A') {
            rules['A'] += (choice == 0) ? "A[+A]A[-A]" : "A[-A][+A]A";
        }
        else if (axiom == 'B') {
            rules['B'] += (choice == 0) ? "B[+B]B[-B]" : "B[-B][+B]B";
        }
    }
    else if (guidedStep == 2) {
        // Paso 3: ¿Añadir hojas?
        if (choice == 0) { // Si elige "Sí", se añaden hojas (@)
            if (axiom == 'F') rules['F'] += "[@]";
            if (axiom == 'X') rules['X'] += "[@]";
            if (axiom == 'Y') rules['Y'] += "[@]";
            if (axiom == 'A') rules['A'] += "[@]";
            if (axiom == 'B') rules['B'] += "[@]";
        }
    }

    guidedStep++; // Avanza al siguiente paso
}


void LSystem::resetGuided() {
    guidedStep = 0; // Reiniciar los pasos guiados
    rules.clear();
    current = originalAxiom;
}