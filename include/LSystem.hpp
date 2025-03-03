#ifndef __LSYSTEM_HPP__
#define __LSYSTEM_HPP__ 1

#include <string>
#include <vector>
#include <map>

class LSystem {
private:
    std::string originalAxiom;
    std::string current;
    std::map<char, std::string> rules;

public:
    LSystem(const std::string& axiom, const std::vector<std::pair<char, std::string>>& rules);
    void iterate();
    void generate(int iterations);
    std::string getCurrent() const;
    void reset();
    int iterations;

    struct Axiom {
        char letra;
        std::string regla;
    };

    static std::vector<Axiom> InitAxiomMap(std::vector<Axiom>& axiomMap);

};

#endif