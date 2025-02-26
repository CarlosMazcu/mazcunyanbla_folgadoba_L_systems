#include "../include/LSystem.hpp"

LSystem::LSystem(const std::string& axiom, const std::vector<std::pair<char, std::string>>& rules)
    : originalAxiom(axiom), current(axiom) {
    for (const auto& rule : rules) {
        this->rules[rule.first] = rule.second;
    }
}

void LSystem::iterate() {
    std::string newCurrent;
    for (char c : current) {
        if (rules.find(c) != rules.end()) {
            newCurrent += rules[c];
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

std::vector<LSystem::Axiom> LSystem::InitAxiomMap(std::vector<Axiom>& axiomMap)
{
    axiomMap = {
        {'F', "F[+F][-F]"},  // Árbol fractal básico
        {'X', "F-[[X]+X]+F[+FX]-X"},  // Helecho fractal
        {'G', "G[+F]-G"},  // Crecimiento ramificado
        {'Y', "F+F--F+F"},  // Copo de nieve de Koch (F en lugar de Y)
        {'Z', "F[-F]+F"}    // Estructura coral
    };
    return axiomMap;
}