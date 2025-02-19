#ifndef __LSYSTEM_H__
#define __LSYSTEM_H__ 1

#include <string>
#include <unordered_map>
#include <vector>

class LSystem {
public:
    LSystem(const std::string& axiom, const std::vector<std::pair<char, std::string>>& rules);

    // Aplica las reglas de producción una vez
    void iterate();
    // Aplica las reglas un número específico de veces
    void generate(int iterations) {
        for (int i = 0; i < iterations; ++i) {
            iterate();
        }
    }

    // Devuelve la cadena actual
    std::string getCurrent() const {
        return current;
    }

private:
    std::string axiom;
    std::unordered_map<char, std::string> rules;
    std::string current;
};

#endif // LSYSTEM_H