#include "../include/LSystem.hpp"


LSystem::LSystem(const std::string& axiom, const std::vector<std::pair<char, std::string>>& rules)
    : axiom(axiom), current(axiom) {
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
