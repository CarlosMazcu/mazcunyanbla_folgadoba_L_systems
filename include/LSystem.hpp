#ifndef __LSYSTEM_HPP__
#define __LSYSTEM_HPP__ 1

#include <string>
#include <vector>
#include <unordered_map>


class LSystem {
private:
    std::string originalAxiom;
    std::string current;
    std::unordered_map<char, std::string> rules;

public:
    LSystem(const std::string& axiom, const std::unordered_map<char, std::string>& rules);

    void iterate();
    void generate(int iterations);
    std::string getCurrent() const;
    void reset();
    int iterations;

    //static std::vector<Axiom> InitAxiomMap(std::vector<Axiom>& axiomMap);

};

#endif