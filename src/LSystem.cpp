#include "../include/LSystem.hpp"

LSystem::LSystem(const std::string& axiom, const std::unordered_map<char, std::string>& rules)
    : originalAxiom(axiom), current(axiom), rules(rules), iterations(4) {}

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

std::string LSystem::getCurrent() const {
    return current;
}

void LSystem::reset() {
    current = originalAxiom;
}
//std::vector<LSystem::Axiom> LSystem::InitAxiomMap(std::vector<Axiom>& axiomMap) {
//    axiomMap = {
//        {'F', "F[+F][-F]"},// Árbol fractal básico
//        {'F', "F[+F][-F]F@"},// Árbol básico con ramas y hojas
//        {'X', "F[+X][-X]FXF@"},  // Árbol complejo con ramas y hojas
//        {'X', "F[+X][-X]FX"}, // Axiom más complejo con ramificación adicional
//        {'F', "F[+F]F[-F]F@"},  // Arbol con ramas densas y hojas
//        {'F', "FF+[+F-F-F]-[-F+F+F]"},
//        {'F', "F[+F]F[-F]F[#]"},
//
//    };
//    return axiomMap;
//}

